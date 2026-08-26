#include <gtest/gtest.h>

#include <cstdint>
#include <set>
#include <string>
#include <string_view>

#include "fchat/common.hpp"

namespace {

// Independent oracle: re-derives the wire value WITHOUT calling make_opcode
// and WITHOUT the &0x1F mask. Valid only for A-Z inputs, which all protocol
// opcodes are. Used purely to compute expected values against which
// make_opcode is asserted.
constexpr auto ref_opcode(std::string_view s) -> std::uint16_t {
  const auto idx = [](char c) {
    return static_cast<std::uint16_t>(static_cast<unsigned char>(c) - 'A');
  };
  return static_cast<std::uint16_t>((idx(s[2]) << 10) | (idx(s[1]) << 5) |
                                    idx(s[0]));
}

constexpr auto field_low(std::uint16_t v) -> std::uint16_t { return v & 0x1Fu; }
constexpr auto field_mid(std::uint16_t v) -> std::uint16_t {
  return (v >> 5) & 0x1Fu;
}
constexpr auto field_high(std::uint16_t v) -> std::uint16_t {
  return (v >> 10) & 0x1Fu;
}

}  // namespace

// ------------------------------------------------------------------------------
// Common_MakeOpcodeKnownValues : exhaustive known-value check over AAA..ZZZ
// ------------------------------------------------------------------------------
TEST(Common_MakeOpcodeKnownValues, AllTriplesAAAtoZZZ) {
  char buf[3];
  for (char a = 'A'; a <= 'Z'; ++a) {
    buf[0] = a;
    for (char b = 'A'; b <= 'Z'; ++b) {
      buf[1] = b;
      for (char c = 'A'; c <= 'Z'; ++c) {
        buf[2] = c;
        const std::string_view sv(buf, 3);
        EXPECT_EQ(fchat::make_opcode(sv), ref_opcode(sv)) << "triple " << sv;
      }
    }
  }
}

// ------------------------------------------------------------------------------
// Common_MakeOpcodeNoCollisions : injectivity over AAA..ZZZ
// ------------------------------------------------------------------------------
TEST(Common_MakeOpcodeNoCollisions, AllTriplesAAAtoZZZ) {
  std::set<std::uint16_t> seen;
  char buf[3];
  for (char a = 'A'; a <= 'Z'; ++a) {
    buf[0] = a;
    for (char b = 'A'; b <= 'Z'; ++b) {
      buf[1] = b;
      for (char c = 'A'; c <= 'Z'; ++c) {
        buf[2] = c;
        const std::string_view sv(buf, 3);
        ASSERT_TRUE(seen.insert(fchat::make_opcode(sv)).second)
            << "collision on triple " << sv;
      }
    }
  }
  EXPECT_EQ(seen.size(), 26u * 26u * 26u);
}

// ------------------------------------------------------------------------------
// Common_MakeOpcodeIsConstexprAndNoexcept
// ------------------------------------------------------------------------------
TEST(Common_MakeOpcodeIsConstexprAndNoexcept, ConstexprKnownValue) {
  static_assert(fchat::make_opcode("ABC") == ((2u << 10) | (1u << 5) | 0u),
                "make_opcode is not usable in a constant expression");
  static_assert(fchat::make_opcode("IDN") == ref_opcode("IDN"),
                "make_opcode is not usable in a constant expression");
  static_assert(fchat::make_opcode("ERR") == ref_opcode("ERR"),
                "make_opcode is not usable in a constant expression");
}

TEST(Common_MakeOpcodeIsConstexprAndNoexcept, IsNoexcept) {
  static_assert(noexcept(fchat::make_opcode(std::string_view{"ABC"})),
                "make_opcode must be declared noexcept");
}

TEST(Common_MakeOpcodeIsConstexprAndNoexcept, RuntimeMatches) {
  EXPECT_EQ(fchat::make_opcode("ABC"), ref_opcode("ABC"));
  EXPECT_EQ(fchat::make_opcode("PIN"), ref_opcode("PIN"));
  EXPECT_EQ(fchat::make_opcode("HLO"), ref_opcode("HLO"));
}

// ------------------------------------------------------------------------------
// Common_MakeOpcodeCharacterPositionEncoding
// ------------------------------------------------------------------------------
TEST(Common_MakeOpcodeCharacterPositionEncoding, FirstCharOnly) {
  constexpr std::uint16_t base = ref_opcode("ABC");
  constexpr std::uint16_t alt = ref_opcode("XBC");
  EXPECT_NE(field_low(base), field_low(alt));
  EXPECT_EQ(field_mid(base), field_mid(alt));
  EXPECT_EQ(field_high(base), field_high(alt));
  EXPECT_EQ(fchat::make_opcode("XBC"), alt);
}

TEST(Common_MakeOpcodeCharacterPositionEncoding, SecondCharOnly) {
  constexpr std::uint16_t base = ref_opcode("ABC");
  constexpr std::uint16_t alt = ref_opcode("AXC");
  EXPECT_EQ(field_low(base), field_low(alt));
  EXPECT_NE(field_mid(base), field_mid(alt));
  EXPECT_EQ(field_high(base), field_high(alt));
  EXPECT_EQ(fchat::make_opcode("AXC"), alt);
}

TEST(Common_MakeOpcodeCharacterPositionEncoding, ThirdCharOnly) {
  constexpr std::uint16_t base = ref_opcode("ABC");
  constexpr std::uint16_t alt = ref_opcode("ABX");
  EXPECT_EQ(field_low(base), field_low(alt));
  EXPECT_EQ(field_mid(base), field_mid(alt));
  EXPECT_NE(field_high(base), field_high(alt));
  EXPECT_EQ(fchat::make_opcode("ABX"), alt);
}

// ------------------------------------------------------------------------------
// Common_MakeOpcodeAlphabetBoundaries
// ------------------------------------------------------------------------------
TEST(Common_MakeOpcodeAlphabetBoundaries, UppercaseA) {
  EXPECT_EQ(fchat::make_opcode("AAA"), 0u);
  EXPECT_EQ(ref_opcode("AAA"), 0u);
}

TEST(Common_MakeOpcodeAlphabetBoundaries, UppercaseZ) {
  constexpr std::uint16_t expected = (25u << 10) | (25u << 5) | 25u;
  EXPECT_EQ(fchat::make_opcode("ZZZ"), expected);
}

TEST(Common_MakeOpcodeAlphabetBoundaries, LowercaseCollidesWithUppercase) {
  // make_opcode masks each index with 0x1F. ('a' - 'A') == 32, and
  // 32 & 0x1F == 0, so lowercase 'a' produces the same field as 'A'.
  // This documents the intentional masking (no rejection) behavior.
  EXPECT_EQ(fchat::make_opcode("aaa"), fchat::make_opcode("AAA"));
  EXPECT_EQ(fchat::make_opcode("aBC"), fchat::make_opcode("ABC"));
}

TEST(Common_MakeOpcodeAlphabetBoundaries, PunctuationMasks) {
  // '[' == 91 -> 91 - 'A' == 26, 26 & 0x1F == 26.
  constexpr std::uint16_t expected = (26u << 10) | (26u << 5) | 26u;
  EXPECT_EQ(fchat::make_opcode("[[["), expected);
}

TEST(Common_MakeOpcodeAlphabetBoundaries, OutOfRangeMasks) {
  // Byte 0xFF: 255 - 65 == 190, 190 & 0x1F == 30.
  const char c = static_cast<char>(0xFF);
  const std::string s(3, c);
  constexpr std::uint16_t expected = (30u << 10) | (30u << 5) | 30u;
  EXPECT_EQ(fchat::make_opcode(std::string_view(s)), expected);
}

// ------------------------------------------------------------------------------
// Common_MakeOpcodeInputContract
// ------------------------------------------------------------------------------
TEST(Common_MakeOpcodeInputContract, ReadsOnlyFirstThree) {
  // The fourth character of a longer string must be ignored.
  EXPECT_EQ(fchat::make_opcode("ABCD"), fchat::make_opcode("ABC"));
}

TEST(Common_MakeOpcodeInputContract, LengthFourString) {
  EXPECT_EQ(fchat::make_opcode(std::string("ABCD")), ref_opcode("ABC"));
}

TEST(Common_MakeOpcodeInputContract, EmbeddedNulStringView) {
  // Explicit-length string_view whose third byte is an embedded NUL.
  // make_opcode reads only the first three bytes: 'A', 'B', '\0'.
  // Third field: (0 - 'A') as uint16_t wraps to 65471, & 0x1F == 31.
  char buf[] = {'A', 'B', '\0', 'C'};
  const std::string_view sv(buf, 4);
  constexpr std::uint16_t expected = (31u << 10) | (1u << 5) | 0u;
  EXPECT_EQ(fchat::make_opcode(sv), expected);
}

TEST(Common_MakeOpcodeInputContract, NonAsciiBytes) {
  // Non-ASCII byte 0x80: 128 - 65 == 63, 63 & 0x1F == 31.
  char buf[] = {static_cast<char>(0x80), 'B', 'C'};
  const std::string_view sv(buf, 3);
  constexpr std::uint16_t expected = (2u << 10) | (1u << 5) | 31u;
  EXPECT_EQ(fchat::make_opcode(sv), expected);
}

TEST(Common_MakeOpcodeInputContract, ShortStringTerminates) {
  // make_opcode is declared noexcept but reads str.at(0/1/2). A string
  // shorter than three characters therefore violates the contract and,
  // because the function is noexcept, calls std::terminate rather than
  // propagating a catchable std::out_of_range. This documents the actual
  // (terminating) behavior for under-length input.
  EXPECT_DEATH(fchat::make_opcode("AB"), ".*");
  EXPECT_DEATH(fchat::make_opcode(std::string_view("A")), ".*");
}

// ------------------------------------------------------------------------------
// Common_HasPermission
// ------------------------------------------------------------------------------
TEST(Common_HasPermission, SingleFlagSet) {
  EXPECT_TRUE(fchat::has_permission(
      static_cast<std::uint32_t>(fchat::Permission::Admin),
      fchat::Permission::Admin));
}

TEST(Common_HasPermission, SingleFlagClear) {
  EXPECT_FALSE(fchat::has_permission(
      static_cast<std::uint32_t>(fchat::Permission::Admin),
      fchat::Permission::ChatChatop));
}

TEST(Common_HasPermission, ZeroMask) {
  EXPECT_FALSE(fchat::has_permission(0u, fchat::Permission::Admin));
  EXPECT_FALSE(fchat::has_permission(0u, fchat::Permission::Developer));
}

TEST(Common_HasPermission, CombinedMaskContainsFlag) {
  constexpr std::uint32_t mask =
      static_cast<std::uint32_t>(fchat::Permission::Admin) |
      static_cast<std::uint32_t>(fchat::Permission::ChatChatop);
  EXPECT_TRUE(fchat::has_permission(mask, fchat::Permission::ChatChatop));
  EXPECT_TRUE(fchat::has_permission(mask, fchat::Permission::Admin));
}

TEST(Common_HasPermission, CombinedMaskExcludesFlag) {
  constexpr std::uint32_t mask =
      static_cast<std::uint32_t>(fchat::Permission::Admin) |
      static_cast<std::uint32_t>(fchat::Permission::ChatChatop);
  EXPECT_FALSE(fchat::has_permission(mask, fchat::Permission::Developer));
}

TEST(Common_HasPermission, MultiBitMask) {
  constexpr std::uint32_t mask =
      static_cast<std::uint32_t>(fchat::Permission::Admin) |
      static_cast<std::uint32_t>(fchat::Permission::Developer) |
      static_cast<std::uint32_t>(fchat::Permission::FormerStaff);
  EXPECT_TRUE(fchat::has_permission(mask, fchat::Permission::Admin));
  EXPECT_TRUE(fchat::has_permission(mask, fchat::Permission::Developer));
  EXPECT_TRUE(fchat::has_permission(mask, fchat::Permission::FormerStaff));
  EXPECT_FALSE(fchat::has_permission(mask, fchat::Permission::Tester));
}

// ------------------------------------------------------------------------------
// Common_MakeOpcodeEnumConsistency : each protocol enumerator matches
// make_opcode
// ------------------------------------------------------------------------------
#define CHECK_C2S(NAME)                                           \
  TEST(Common_MakeOpcodeEnumConsistency, C2S_##NAME) {            \
    EXPECT_EQ(static_cast<std::uint16_t>(fchat::C2SOpCode::NAME), \
              fchat::make_opcode(#NAME));                         \
  }

#define CHECK_S2C(NAME)                                           \
  TEST(Common_MakeOpcodeEnumConsistency, S2C_##NAME) {            \
    EXPECT_EQ(static_cast<std::uint16_t>(fchat::S2COpCode::NAME), \
              fchat::make_opcode(#NAME));                         \
  }

CHECK_C2S(ACB)
CHECK_C2S(AOP)
CHECK_C2S(AWC)
CHECK_C2S(BRO)
CHECK_C2S(CBL)
CHECK_C2S(CBU)
CHECK_C2S(CCR)
CHECK_C2S(CDS)
CHECK_C2S(CHA)
CHECK_C2S(CIU)
CHECK_C2S(CKU)
CHECK_C2S(COA)
CHECK_C2S(COL)
CHECK_C2S(COR)
CHECK_C2S(CRC)
CHECK_C2S(CSO)
CHECK_C2S(CTU)
CHECK_C2S(CUB)
CHECK_C2S(DOP)
CHECK_C2S(FKS)
CHECK_C2S(IDN)
CHECK_C2S(IGN)
CHECK_C2S(JCH)
CHECK_C2S(KIC)
CHECK_C2S(KIK)
CHECK_C2S(KIN)
CHECK_C2S(LCH)
CHECK_C2S(LRP)
CHECK_C2S(MSG)
CHECK_C2S(ORS)
CHECK_C2S(PIN)
CHECK_C2S(PRI)
CHECK_C2S(PRO)
CHECK_C2S(RLD)
CHECK_C2S(RLL)
CHECK_C2S(RMO)
CHECK_C2S(RST)
CHECK_C2S(RWD)
CHECK_C2S(SFC)
CHECK_C2S(STA)
CHECK_C2S(TMO)
CHECK_C2S(TPN)
CHECK_C2S(UNB)
CHECK_C2S(UPT)

CHECK_S2C(ADL)
CHECK_S2C(AOP)
CHECK_S2C(BRO)
CHECK_S2C(CBU)
CHECK_S2C(CDS)
CHECK_S2C(CHA)
CHECK_S2C(CIU)
CHECK_S2C(CKU)
CHECK_S2C(COA)
CHECK_S2C(COL)
CHECK_S2C(CON)
CHECK_S2C(COR)
CHECK_S2C(CSO)
CHECK_S2C(CTU)
CHECK_S2C(DOP)
CHECK_S2C(ERR)
CHECK_S2C(FKS)
CHECK_S2C(FLN)
CHECK_S2C(HLO)
CHECK_S2C(FRL)
CHECK_S2C(ICH)
CHECK_S2C(IDN)
CHECK_S2C(IGN)
CHECK_S2C(JCH)
CHECK_S2C(KID)
CHECK_S2C(LCH)
CHECK_S2C(LIS)
CHECK_S2C(LRP)
CHECK_S2C(MSG)
CHECK_S2C(NLN)
CHECK_S2C(ORS)
CHECK_S2C(PIN)
CHECK_S2C(PRD)
CHECK_S2C(PRI)
CHECK_S2C(RLL)
CHECK_S2C(RMO)
CHECK_S2C(RTB)
CHECK_S2C(SFC)
CHECK_S2C(STA)
CHECK_S2C(SYS)
CHECK_S2C(TPN)
CHECK_S2C(UPT)
CHECK_S2C(VAR)
