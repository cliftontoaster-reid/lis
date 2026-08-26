#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <set>
#include <thread>
#include <vector>

#include "fchat/detail/mpsc.hpp"

namespace {

using fchat::detail::MpscQueue;

// ---------------------------------------------------------------------------
// Single-threaded / logic behavior
// ---------------------------------------------------------------------------

TEST(MpscQueue, Construct_UpToCapacitySucceeds) {
  MpscQueue<int> q(3);
  EXPECT_TRUE(q.push(1));
  EXPECT_TRUE(q.push(2));
  EXPECT_TRUE(q.push(3));
  EXPECT_FALSE(q.push(4));  // full
}

TEST(MpscQueue, Construct_CapacityZeroAlwaysFull) {
  MpscQueue<int> q(0);
  EXPECT_FALSE(q.push(1));
  EXPECT_FALSE(q.push(2));
}

TEST(MpscQueue, Push_LValueSucceeds) {
  MpscQueue<int> q(4);
  int x = 5;
  EXPECT_TRUE(q.push(x));
  std::vector<int> out;
  q.drain(out);
  EXPECT_EQ(out, std::vector<int>{5});
}

TEST(MpscQueue, Push_RValueSucceeds) {
  MpscQueue<int> q(4);
  EXPECT_TRUE(q.push(5));
  std::vector<int> out;
  q.drain(out);
  EXPECT_EQ(out, std::vector<int>{5});
}

TEST(MpscQueue, Push_PreservesOrder) {
  MpscQueue<int> q(8);
  q.push(1);
  q.push(2);
  q.push(3);
  std::vector<int> out;
  q.drain(out);
  EXPECT_EQ(out, (std::vector<int>{1, 2, 3}));
}

TEST(MpscQueue, Drain_ReturnsPushedItems) {
  MpscQueue<int> q(8);
  q.push(10);
  q.push(20);
  std::vector<int> out;
  const bool done = q.drain(out);
  EXPECT_FALSE(done);  // open
  EXPECT_EQ(out, (std::vector<int>{10, 20}));
}

TEST(MpscQueue, Drain_ClearsInternalBuffer) {
  MpscQueue<int> q(8);
  q.push(1);
  std::vector<int> out;
  q.drain(out);
  // Internal buffer is now empty, so a push should succeed again.
  EXPECT_TRUE(q.push(2));
}

TEST(MpscQueue, Drain_ReturnsFalseWhileOpen) {
  MpscQueue<int> q(8);
  q.push(1);
  std::vector<int> out;
  const bool done = q.drain(out);
  EXPECT_FALSE(done);
}

TEST(MpscQueue, Drain_ReturnsTrueWhenClosedAndEmpty) {
  MpscQueue<int> q(8);
  q.close();
  std::vector<int> out;
  const bool done = q.drain(out);
  EXPECT_TRUE(done);
  EXPECT_TRUE(out.empty());
}

TEST(MpscQueue, Drain_ReturnsFalseWhenClosedWithData) {
  MpscQueue<int> q(8);
  q.push(42);
  q.close();
  std::vector<int> out;
  const bool done1 = q.drain(out);
  EXPECT_FALSE(done1);  // closed but data still pending
  EXPECT_EQ(out, std::vector<int>{42});

  std::vector<int> out2;
  const bool done2 = q.drain(out2);
  EXPECT_TRUE(done2);  // now closed and empty
  EXPECT_TRUE(out2.empty());
}

TEST(MpscQueue, Push_AfterCloseFails) {
  MpscQueue<int> q(8);
  q.close();
  EXPECT_FALSE(q.push(1));
}

TEST(MpscQueue, Close_MultipleCallsSafe) {
  MpscQueue<int> q(8);
  q.close();
  q.close();  // idempotent, must not crash or double-signal
  EXPECT_FALSE(q.push(1));
}

TEST(MpscQueue, TryDrain_SwapsWhenUncontended) {
  MpscQueue<int> q(8);
  q.push(7);
  std::vector<int> out;
  EXPECT_TRUE(q.try_drain(out));
  EXPECT_EQ(out, std::vector<int>{7});
}

TEST(MpscQueue, TryDrain_EmptyQueueReturnsTrue) {
  MpscQueue<int> q(8);
  std::vector<int> out;
  EXPECT_TRUE(q.try_drain(out));
  EXPECT_TRUE(out.empty());
}

TEST(MpscQueue, TryDrain_SwapDiscardsPriorOut) {
  MpscQueue<int> q(8);
  std::vector<int> out = {99};
  q.push(1);
  EXPECT_TRUE(q.try_drain(out));
  // try_drain SWAPS: prior out contents lost into the queue, out holds queue data
  EXPECT_EQ(out, std::vector<int>{1});

  std::vector<int> out2;
  EXPECT_TRUE(q.try_drain(out2));
  EXPECT_EQ(out2, std::vector<int>{99});  // the discarded 99 is now in the queue
}

TEST(MpscQueue, Drain_ClearsPriorOut) {
  MpscQueue<int> q(8);
  std::vector<int> out = {99};
  q.push(1);
  q.drain(out);  // drain clears out first, then swaps
  EXPECT_EQ(out, std::vector<int>{1});
}

// ---------------------------------------------------------------------------
// Concurrency behavior
// ---------------------------------------------------------------------------

TEST(MpscQueue_Concurrency, SingleProducerSingleConsumer) {
  MpscQueue<int> q(4096);
  std::vector<int> collected;

  std::thread producer([&q] {
    for (int i = 0; i < 1000; ++i) q.push(i);
    q.close();
  });

  std::vector<int> out;
  while (true) {
    const bool done = q.drain(out);
    collected.insert(collected.end(), out.begin(), out.end());
    if (done) break;
  }
  producer.join();

  ASSERT_EQ(collected.size(), 1000u);
  std::sort(collected.begin(), collected.end());
  for (int i = 0; i < 1000; ++i) EXPECT_EQ(collected[i], i);
}

TEST(MpscQueue_Concurrency, MultipleProducersNoLoss) {
  MpscQueue<long long> q(1'000'000);
  std::vector<std::thread> producers;
  for (int t = 0; t < 4; ++t) {
    producers.emplace_back([&q, t] {
      for (int i = 0; i < 250; ++i) q.push(static_cast<long long>(t) * 100000 + i);
    });
  }
  for (auto& p : producers) p.join();
  q.close();

  std::vector<long long> collected;
  std::vector<long long> out;
  while (true) {
    const bool done = q.drain(out);
    collected.insert(collected.end(), out.begin(), out.end());
    if (done) break;
  }

  EXPECT_EQ(collected.size(), 1000u);
  std::set<long long> unique(collected.begin(), collected.end());
  EXPECT_EQ(unique.size(), 1000u);
}

TEST(MpscQueue_Concurrency, CloseWakesBlockedDrain) {
  MpscQueue<int> q(16);
  bool done = false;
  std::thread consumer([&q, &done] {
    std::vector<int> out;
    done = q.drain(out);  // blocks until close()
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  q.close();
  consumer.join();
  EXPECT_TRUE(done);
}

TEST(MpscQueue_Concurrency, PushAfterCloseFromThreadFails) {
  MpscQueue<int> q(16);
  q.close();
  std::atomic<bool> any_success{false};
  std::thread pusher([&q, &any_success] {
    for (int i = 0; i < 100; ++i) {
      if (q.push(i)) any_success.store(true);
    }
  });
  pusher.join();
  EXPECT_FALSE(any_success);
}

TEST(MpscQueue_Concurrency, TryDrainUnderContentionReturnsFalse) {
  MpscQueue<int> q(1024);
  std::atomic<bool> saw_false{false};

  std::thread producer([&q] {
    const auto end =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    int i = 0;
    while (std::chrono::steady_clock::now() < end) q.push(i++);
  });

  const auto end =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
  std::vector<int> out;
  while (std::chrono::steady_clock::now() < end) {
    if (!q.try_drain(out)) saw_false.store(true);
  }
  producer.join();
  EXPECT_TRUE(saw_false);
}

TEST(MpscQueue_Concurrency, NoDataLossStress) {
  MpscQueue<long long> q(10'000'000);
  std::vector<std::thread> producers;
  for (int t = 0; t < 8; ++t) {
    producers.emplace_back([&q, t] {
      for (int i = 0; i < 10000; ++i) {
        q.push(static_cast<long long>(t) * 100000 + i);
      }
    });
  }
  for (auto& p : producers) p.join();
  q.close();

  std::vector<long long> collected;
  std::vector<long long> out;
  while (true) {
    const bool done = q.drain(out);
    collected.insert(collected.end(), out.begin(), out.end());
    if (done) break;
  }

  EXPECT_EQ(collected.size(), 80000u);
  std::set<long long> unique(collected.begin(), collected.end());
  EXPECT_EQ(unique.size(), 80000u);
}

}  // namespace
