const childProcess = require("child_process");
const fs = require("fs");
const path = require("path");

const root = fs.existsSync(path.resolve(process.cwd(), "csend.sln"))
  ? path.resolve(process.cwd())
  : path.resolve(__dirname, "..");
const tmpDir = path.join(root, ".tmp");

function loadLocalEnv() {
  const envPath = path.join(root, ".build.env");
  if (!fs.existsSync(envPath)) return;

  const lines = fs.readFileSync(envPath, "utf8").split(/\r?\n/);
  for (const line of lines) {
    const trimmed = line.trim();
    if (!trimmed || trimmed.startsWith("#")) continue;

    const eq = trimmed.indexOf("=");
    if (eq < 1) continue;

    const key = trimmed.slice(0, eq).trim();
    let value = trimmed.slice(eq + 1).trim();
    if (
      (value.startsWith('"') && value.endsWith('"')) ||
      (value.startsWith("'") && value.endsWith("'"))
    ) {
      value = value.slice(1, -1);
    }

    if (!process.env[key]) {
      process.env[key] = value;
    }
  }
}

function existingPath(...parts) {
  const candidate = path.join(...parts);
  return fs.existsSync(candidate) ? candidate : null;
}

function findMsbuild() {
  if (process.env.CSEND_MSBUILD && fs.existsSync(process.env.CSEND_MSBUILD)) {
    return process.env.CSEND_MSBUILD;
  }

  if (process.env.CSEND_VS_INSTALL) {
    const msbuild = path.join(
      process.env.CSEND_VS_INSTALL,
      "MSBuild",
      "Current",
      "Bin",
      "MSBuild.exe"
    );
    if (fs.existsSync(msbuild)) return msbuild;
  }

  const vswhere = existingPath(
    process.env.CSEND_VSWHERE ||
      path.join(
        process.env["ProgramFiles(x86)"] || "C:\\Program Files (x86)",
        "Microsoft Visual Studio",
        "Installer",
        "vswhere.exe"
      )
  );

  if (vswhere) {
    const result = childProcess.spawnSync(
      vswhere,
      [
        "-latest",
        "-products",
        "*",
        "-requires",
        "Microsoft.Component.MSBuild",
        "-property",
        "installationPath",
      ],
      { encoding: "utf8" }
    );

    if (result.status === 0 && result.stdout.trim()) {
      const installPath = result.stdout.trim().split(/\r?\n/)[0];
      const msbuild = path.join(installPath, "MSBuild", "Current", "Bin", "MSBuild.exe");
      if (fs.existsSync(msbuild)) return msbuild;
    }
  }

  const programFiles = process.env.ProgramFiles || "C:\\Program Files";
  const programFilesX86 = process.env["ProgramFiles(x86)"] || "C:\\Program Files (x86)";
  const candidates = [
    path.join(programFiles, "Microsoft Visual Studio", "18", "Community"),
    path.join(programFiles, "Microsoft Visual Studio", "18", "BuildTools"),
    path.join(programFiles, "Microsoft Visual Studio", "2022", "Community"),
    path.join(programFiles, "Microsoft Visual Studio", "2022", "BuildTools"),
    path.join(programFilesX86, "Microsoft Visual Studio", "2022", "Community"),
    path.join(programFilesX86, "Microsoft Visual Studio", "2022", "BuildTools"),
  ];

  for (const candidate of candidates) {
    const msbuild = path.join(candidate, "MSBuild", "Current", "Bin", "MSBuild.exe");
    if (fs.existsSync(msbuild)) return msbuild;
  }

  return null;
}

function setIfPresent(env, name) {
  if (process.env[name]) env[name] = process.env[name];
}

function makeBuildEnv() {
  const systemRoot = process.env.SystemRoot || process.env.WINDIR || "C:\\Windows";
  const env = {
    SystemRoot: systemRoot,
    WINDIR: process.env.WINDIR || systemRoot,
    ComSpec: process.env.ComSpec || path.join(systemRoot, "System32", "cmd.exe"),
    TEMP: tmpDir,
    TMP: tmpDir,
    PROCESSOR_ARCHITECTURE: process.env.PROCESSOR_ARCHITECTURE || "AMD64",
    NUMBER_OF_PROCESSORS: process.env.NUMBER_OF_PROCESSORS || "1",
    Path:
      process.env.CSEND_BUILD_PATH ||
      [
        path.join(systemRoot, "System32"),
        systemRoot,
        path.join(systemRoot, "System32", "Wbem"),
        path.join(systemRoot, "System32", "WindowsPowerShell", "v1.0"),
      ].join(";"),
  };

  [
    "APPDATA",
    "LOCALAPPDATA",
    "ProgramData",
    "ProgramFiles",
    "ProgramFiles(x86)",
    "USERPROFILE",
  ].forEach((name) => setIfPresent(env, name));

  if (process.env.CSEND_BUILD_TEMP) {
    env.TEMP = process.env.CSEND_BUILD_TEMP;
    env.TMP = process.env.CSEND_BUILD_TEMP;
  }

  return env;
}

function parseArgs(argv) {
  const parsed = {
    configuration: "Debug",
    platform: "x64",
    extra: [],
  };

  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (arg === "--config" || arg === "-c") {
      parsed.configuration = argv[++i] || parsed.configuration;
    } else if (arg === "--platform" || arg === "-p") {
      parsed.platform = argv[++i] || parsed.platform;
    } else if (arg === "--") {
      parsed.extra.push(...argv.slice(i + 1));
      break;
    } else if (/^(Debug|Release)$/i.test(arg)) {
      parsed.configuration = arg;
    } else {
      parsed.extra.push(arg);
    }
  }

  return parsed;
}

loadLocalEnv();

fs.mkdirSync(process.env.CSEND_BUILD_TEMP || tmpDir, { recursive: true });

const msbuild = findMsbuild();
if (!msbuild) {
  console.error("MSBuild was not found.");
  console.error("Install Visual Studio Build Tools, or set CSEND_MSBUILD / CSEND_VS_INSTALL.");
  process.exit(1);
}

const options = parseArgs(process.argv.slice(2));
const args = [
  "csend.sln",
  "/m",
  `/p:Configuration=${options.configuration}`,
  `/p:Platform=${options.platform}`,
  "/nr:false",
  ...options.extra,
];

console.log(`MSBuild: ${msbuild}`);
console.log(`Config:  ${options.configuration}|${options.platform}`);

const child = childProcess.spawn(msbuild, args, {
  cwd: root,
  env: makeBuildEnv(),
  stdio: "inherit",
});

child.on("exit", (code) => process.exit(code));
child.on("error", (error) => {
  console.error(error.message);
  process.exit(1);
});
