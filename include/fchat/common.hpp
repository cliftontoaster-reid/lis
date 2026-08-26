#pragma once

#include <cstdint>
#include <glaze/core/opts.hpp>
#include <glaze/glaze.hpp>  // IWYU pragma: keep
#include <string_view>

namespace fchat {

inline constexpr std::uint16_t kCharMask = 0x1F;
inline constexpr std::uint16_t kCharBits = 5;
inline constexpr std::uint16_t kCharShift = kCharBits * 2;

// Precondition: str has exactly 3 characters, each in the range 'A'..'Z'.
constexpr auto make_opcode(std::string_view str) noexcept -> std::uint16_t {
  const std::uint16_t first =
      static_cast<std::uint16_t>(static_cast<unsigned char>(str.at(0)) - 'A') &
      kCharMask;
  const std::uint16_t second =
      static_cast<std::uint16_t>(static_cast<unsigned char>(str.at(1)) - 'A') &
      kCharMask;
  const std::uint16_t third =
      static_cast<std::uint16_t>(static_cast<unsigned char>(str.at(2)) - 'A') &
      kCharMask;
  return (third << kCharShift) | (second << kCharBits) | first;
}

enum class C2SOpCode : std::uint16_t {
  ACB = make_opcode("ACB"),
  AOP = make_opcode("AOP"),
  AWC = make_opcode("AWC"),
  BRO = make_opcode("BRO"),
  CBL = make_opcode("CBL"),
  CBU = make_opcode("CBU"),
  CCR = make_opcode("CCR"),
  CDS = make_opcode("CDS"),
  CHA = make_opcode("CHA"),
  CIU = make_opcode("CIU"),
  CKU = make_opcode("CKU"),
  COA = make_opcode("COA"),
  COL = make_opcode("COL"),
  COR = make_opcode("COR"),
  CRC = make_opcode("CRC"),
  CSO = make_opcode("CSO"),
  CTU = make_opcode("CTU"),
  CUB = make_opcode("CUB"),
  DOP = make_opcode("DOP"),
  FKS = make_opcode("FKS"),
  IDN = make_opcode("IDN"),
  IGN = make_opcode("IGN"),
  JCH = make_opcode("JCH"),
  KIC = make_opcode("KIC"),
  KIK = make_opcode("KIK"),
  KIN = make_opcode("KIN"),
  LCH = make_opcode("LCH"),
  LRP = make_opcode("LRP"),
  MSG = make_opcode("MSG"),
  ORS = make_opcode("ORS"),
  PIN = make_opcode("PIN"),
  PRI = make_opcode("PRI"),
  PRO = make_opcode("PRO"),
  RLD = make_opcode("RLD"),
  RLL = make_opcode("RLL"),
  RMO = make_opcode("RMO"),
  RST = make_opcode("RST"),
  RWD = make_opcode("RWD"),
  SFC = make_opcode("SFC"),
  STA = make_opcode("STA"),
  TMO = make_opcode("TMO"),
  TPN = make_opcode("TPN"),
  UNB = make_opcode("UNB"),
  UPT = make_opcode("UPT"),
};

enum class S2COpCode : std::uint16_t {
  ADL = make_opcode("ADL"),
  AOP = make_opcode("AOP"),
  BRO = make_opcode("BRO"),
  CBU = make_opcode("CBU"),
  CDS = make_opcode("CDS"),
  CHA = make_opcode("CHA"),
  CIU = make_opcode("CIU"),
  CKU = make_opcode("CKU"),
  COA = make_opcode("COA"),
  COL = make_opcode("COL"),
  CON = make_opcode("CON"),
  COR = make_opcode("COR"),
  CSO = make_opcode("CSO"),
  CTU = make_opcode("CTU"),
  DOP = make_opcode("DOP"),
  ERR = make_opcode("ERR"),
  FKS = make_opcode("FKS"),
  FLN = make_opcode("FLN"),
  HLO = make_opcode("HLO"),
  FRL = make_opcode("FRL"),
  ICH = make_opcode("ICH"),
  IDN = make_opcode("IDN"),
  IGN = make_opcode("IGN"),
  JCH = make_opcode("JCH"),
  KID = make_opcode("KID"),
  LCH = make_opcode("LCH"),
  LIS = make_opcode("LIS"),
  LRP = make_opcode("LRP"),
  MSG = make_opcode("MSG"),
  NLN = make_opcode("NLN"),
  ORS = make_opcode("ORS"),
  PIN = make_opcode("PIN"),
  PRD = make_opcode("PRD"),
  PRI = make_opcode("PRI"),
  RLL = make_opcode("RLL"),
  RMO = make_opcode("RMO"),
  RTB = make_opcode("RTB"),
  SFC = make_opcode("SFC"),
  STA = make_opcode("STA"),
  SYS = make_opcode("SYS"),
  TPN = make_opcode("TPN"),
  UPT = make_opcode("UPT"),
  VAR = make_opcode("VAR"),
};

inline constexpr glz::opts fchat_opts{.quoted_num = 1};

enum class ChannelMode : std::int8_t {
  CHAT,
  ADS,
  BOTH,
};

enum class ErrorCode : std::int8_t {
  Success = 0,
  SyntaxError = 1,
  NoFreeSlots = 2,
  RequiresLogin = 3,
  IdentificationFailed = 4,
  ChannelMessageRateLimit = 5,
  CharacterNotFound = 6,
  ProfileRateLimit = 7,
  UnknownCommand = 8,
  BannedFromServer = 9,
  RequiresAdmin = 10,
  AlreadyIdentified = 11,
  KinkRateLimit = 13,
  MessageTooLong = 15,
  AlreadyGlobalMod = 16,
  NotGlobalMod = 17,
  NoSearchResults = 18,
  RequiresMod = 19,
  IgnoredByCharacter = 20,
  ProtectedCharacter = 21,
  ChannelNotFound = 26,
  AlreadyInChannel = 28,
  TooManyIPConnections = 30,
  LoggedOutElsewhere = 31,
  AccountAlreadyBanned = 32,
  UnknownAuthMethod = 33,
  RollError = 36,
  InvalidTimeoutDuration = 38,
  TimedOut = 39,
  Kicked = 40,
  AlreadyBannedFromChannel = 41,
  NotBannedFromChannel = 42,
  InviteOnlyChannel = 44,
  MustBeInChannel = 45,
  CannotInviteToPublic = 47,
  BannedFromChannel = 48,
  UserNotInChannel = 49,
  SearchRateLimit = 50,
  ModCallRateLimit = 54,
  AdRateLimit = 56,
  ChannelAdsOnly = 59,
  ChannelChatOnly = 60,
  TooManySearchTerms = 61,
  NoLoginSlots = 62,
  IgnoreListFull = 64,
  ChannelTitleTooLong = 67,
  TooManySearchResults = 72,

  // The negative panic buttons
  FatalInternalError = -1,
  ProcessingError = -2,
  NotImplementedYet = -3,
  LoginServerTimeout = -4,
  UnknownError = -5,
  FrontpageRollError = -10
};

enum class Permission : std::uint32_t {
  Admin = 0x1,
  ChatChatop = 0x2,
  ChatChanop = 0x4,
  HelpdeskChat = 0x8,
  HelpdeskGeneral = 0x10,
  ModerationSite = 0x20,
  Reserved = 0x40,
  MiscGroupRequests = 0x80,
  MiscNewsposts = 0x100,
  MiscChangelog = 0x200,
  MiscFeatureRequests = 0x400,
  DevBugreports = 0x800,
  DevTags = 0x1000,
  DevKinks = 0x2000,
  Developer = 0x4000,
  Tester = 0x8000,
  Subscriptions = 0x10000,
  FormerStaff = 0x20000
};

[[nodiscard]] constexpr auto has_permission(std::uint32_t mask,
                                            Permission perm) noexcept -> bool {
  return (mask & static_cast<std::uint32_t>(perm)) != 0;
}

}  // namespace fchat