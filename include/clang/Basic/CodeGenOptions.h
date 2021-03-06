//===--- CodeGenOptions.h ---------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the CodeGenOptions interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_BASIC_CODEGENOPTIONS_H
#define LLVM_CLANG_BASIC_CODEGENOPTIONS_H

#include "clang/Basic/DebugInfoOptions.h"
#include "clang/Basic/Sanitizers.h"
#include "llvm/Support/Regex.h"
#include "llvm/Target/TargetOptions.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace clang {

/// \brief Bitfields of CodeGenOptions, split out from CodeGenOptions to ensure
/// that this large collection of bitfields is a trivial class type.
class CodeGenOptionsBase {
public:
#define CODEGENOPT(Name, Bits, Default) unsigned Name : Bits;
#define ENUM_CODEGENOPT(Name, Type, Bits, Default)
#include "clang/Basic/CodeGenOptions.def"

protected:
#define CODEGENOPT(Name, Bits, Default)
#define ENUM_CODEGENOPT(Name, Type, Bits, Default) unsigned Name : Bits;
#include "clang/Basic/CodeGenOptions.def"
};

/// CodeGenOptions - Track various options which control how the code
/// is optimized and passed to the backend.
class CodeGenOptions : public CodeGenOptionsBase {
public:
  enum InliningMethod {
    NoInlining,         // Perform no inlining whatsoever.
    NormalInlining,     // Use the standard function inlining pass.
    OnlyAlwaysInlining  // Only run the always inlining pass.
  };

  enum VectorLibrary {
    NoLibrary, // Don't use any vector library.
    Accelerate // Use the Accelerate framework.
  };

  enum ObjCDispatchMethodKind {
    Legacy = 0,
    NonLegacy = 1,
    Mixed = 2
  };

  enum TLSModel {
    GeneralDynamicTLSModel,
    LocalDynamicTLSModel,
    InitialExecTLSModel,
    LocalExecTLSModel
  };

  enum FPContractModeKind {
    FPC_Off,        // Form fused FP ops only where result will not be affected.
    FPC_On,         // Form fused FP ops according to FP_CONTRACT rules.
    FPC_Fast        // Aggressively fuse FP ops (E.g. FMA).
  };

  enum StructReturnConventionKind {
    SRCK_Default,  // No special option was passed.
    SRCK_OnStack,  // Small structs on the stack (-fpcc-struct-return).
    SRCK_InRegs    // Small structs in registers (-freg-struct-return).
  };

  enum ProfileInstrKind {
    ProfileNone,       // Profile instrumentation is turned off.
    ProfileClangInstr, // Clang instrumentation to generate execution counts
                       // to use with PGO.
    ProfileIRInstr,    // IR level PGO instrumentation in LLVM.
  };

  /// The code model to use (-mcmodel).
  std::string CodeModel;

  /// The filename with path we use for coverage files. The extension will be
  /// replaced.
  std::string CoverageFile;

  /// The version string to put into coverage files.
  char CoverageVersion[4];

  /// Enable additional debugging information.
  std::string DebugPass;

  /// The string to embed in debug information as the current working directory.
  std::string DebugCompilationDir;

  /// The string to embed in the debug information for the compile unit, if
  /// non-empty.
  std::string DwarfDebugFlags;

  std::map<std::string, std::string> DebugPrefixMap;

  /// The ABI to use for passing floating point arguments.
  std::string FloatABI;

  /// The float precision limit to use, if non-empty.
  std::string LimitFloatPrecision;

  /// The name of the bitcode file to link before optzns.
  std::vector<std::pair<unsigned, std::string>> LinkBitcodeFiles;

  /// The user provided name for the "main file", if non-empty. This is useful
  /// in situations where the input file name does not match the original input
  /// file, for example with -save-temps.
  std::string MainFileName;

  /// The name for the split debug info file that we'll break out. This is used
  /// in the backend for setting the name in the skeleton cu.
  std::string SplitDwarfFile;

  /// The name of the relocation model to use.
  std::string RelocationModel;

  /// The thread model to use
  std::string ThreadModel;

  /// If not an empty string, trap intrinsics are lowered to calls to this
  /// function instead of to trap instructions.
  std::string TrapFuncName;

  /// A list of command-line options to forward to the LLVM backend.
  std::vector<std::string> BackendOptions;

  /// A list of dependent libraries.
  std::vector<std::string> DependentLibraries;

  /// A list of linker options to embed in the object file.
  std::vector<std::string> LinkerOptions;

  /// Name of the profile file to use as output for -fprofile-instr-generate
  /// and -fprofile-generate.
  std::string InstrProfileOutput;

  /// Name of the profile file to use with -fprofile-sample-use.
  std::string SampleProfileFile;

  /// Name of the profile file to use as input for -fprofile-instr-use
  std::string ProfileInstrumentUsePath;

  /// Name of the function summary index file to use for ThinLTO function
  /// importing.
  std::string ThinLTOIndexFile;

  /// The EABI version to use
  std::string EABIVersion;

  /// A list of file names passed with -fcuda-include-gpubinary options to
  /// forward to CUDA runtime back-end for incorporating them into host-side
  /// object file.
  std::vector<std::string> CudaGpuBinaryFileNames;

  /// Regular expression to select optimizations for which we should enable
  /// optimization remarks. Transformation passes whose name matches this
  /// expression (and support this feature), will emit a diagnostic
  /// whenever they perform a transformation. This is enabled by the
  /// -Rpass=regexp flag.
  std::shared_ptr<llvm::Regex> OptimizationRemarkPattern;

  /// Regular expression to select optimizations for which we should enable
  /// missed optimization remarks. Transformation passes whose name matches this
  /// expression (and support this feature), will emit a diagnostic
  /// whenever they tried but failed to perform a transformation. This is
  /// enabled by the -Rpass-missed=regexp flag.
  std::shared_ptr<llvm::Regex> OptimizationRemarkMissedPattern;

  /// Regular expression to select optimizations for which we should enable
  /// optimization analyses. Transformation passes whose name matches this
  /// expression (and support this feature), will emit a diagnostic
  /// whenever they want to explain why they decided to apply or not apply
  /// a given transformation. This is enabled by the -Rpass-analysis=regexp
  /// flag.
  std::shared_ptr<llvm::Regex> OptimizationRemarkAnalysisPattern;

  /// Set of files definining the rules for the symbol rewriting.
  std::vector<std::string> RewriteMapFiles;

  /// Set of sanitizer checks that are non-fatal (i.e. execution should be
  /// continued when possible).
  SanitizerSet SanitizeRecover;

  /// Set of sanitizer checks that trap rather than diagnose.
  SanitizerSet SanitizeTrap;

  /// \brief A list of all -fno-builtin-* function names (e.g., memset).
  std::vector<std::string> NoBuiltinFuncs;

  /// List of blacklist files for the whole-program vtable optimization feature.
  std::vector<std::string> WholeProgramVTablesBlacklistFiles;

public:
  // Define accessors/mutators for code generation options of enumeration type.
#define CODEGENOPT(Name, Bits, Default)
#define ENUM_CODEGENOPT(Name, Type, Bits, Default) \
  Type get##Name() const { return static_cast<Type>(Name); } \
  void set##Name(Type Value) { Name = static_cast<unsigned>(Value); }
#include "clang/Basic/CodeGenOptions.def"

  CodeGenOptions();

  /// \brief Is this a libc/libm function that is no longer recognized as a
  /// builtin because a -fno-builtin-* option has been specified?
  bool isNoBuiltinFunc(const char *Name) const;

  const std::vector<std::string> &getNoBuiltinFuncs() const {
    return NoBuiltinFuncs;
  }

  /// \brief Check if Clang profile instrumenation is on.
  bool hasProfileClangInstr() const {
    return getProfileInstr() == ProfileClangInstr;
  }

  /// \brief Check if IR level profile instrumentation is on.
  bool hasProfileIRInstr() const {
    return getProfileInstr() == ProfileIRInstr;
  }

  /// \brief Check if Clang profile use is on.
  bool hasProfileClangUse() const {
    return getProfileUse() == ProfileClangInstr;
  }

  /// \brief Check if IR level profile use is on.
  bool hasProfileIRUse() const {
    return getProfileUse() == ProfileIRInstr;
  }
};

}  // end namespace clang

#endif
