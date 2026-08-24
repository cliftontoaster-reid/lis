param(
    [switch]$Coverage,
    [switch]$Cross
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

$unbound = $args
if ($unbound.Count -gt 0) {
    [Console]::Error.WriteLine('Unknown flag(s): ' + ($unbound -join ' '))
    [Console]::Error.WriteLine('Usage: install-windows.ps1 [-Coverage] [-Cross]')
    exit 1
}

if ($Cross) {
    Write-Host 'Warning: -Cross is not applicable on Windows for this project; continuing.'
}

if ($Coverage) {
    Write-Host 'Warning: -Coverage is not applicable on Windows for this project; continuing.'
}

$principal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
$isAdmin = $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host 'Warning: shell is not elevated. Chocolatey and winget may require an elevated (admin) shell.'
}

Write-Host 'Detecting package manager...'
$choco = Get-Command choco -ErrorAction SilentlyContinue
if ($choco) {
    Write-Host 'Found Chocolatey; installing build toolchain...'
    $required = @()
    foreach ($package in @(
        @{ Name = 'ccache'; Command = 'ccache' },
        @{ Name = 'openssl'; Command = 'openssl' },
        @{ Name = 'curl'; Command = 'curl' },
        @{ Name = 'gnupg'; Command = 'gpg' }
    )) {
        if (-not (Get-Command $package.Command -ErrorAction SilentlyContinue)) {
            $required += $package.Name
        }
    }

    $zlibRoots = @(
        "$env:ProgramFiles\zlib",
        "$env:ChocolateyToolsLocation\zlib",
        "$env:ChocolateyToolsLocation",
        "$env:ProgramData\chocolatey\lib"
    ) | Where-Object { $_ -and (Test-Path $_) }
    $zlibInstalled = $zlibRoots | ForEach-Object {
        $header = Get-ChildItem -Path $_ -Filter 'zlib.h' -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($header) {
            $root = Split-Path (Split-Path $header.FullName -Parent) -Parent
            Get-ChildItem -Path $root -Filter 'zlib*.lib' -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
        }
    } | Select-Object -First 1
    if (-not $zlibInstalled) {
        $required += 'zlib'
    }

    if ($required.Count -gt 0) {
        Write-Host "Installing missing packages via Chocolatey: $($required -join ', ')..."
        choco install -y --no-progress $required
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Warning: choco install exited with code $LASTEXITCODE; continuing anyway."
        }
    } else {
        Write-Host 'All required Chocolatey tools are already available on PATH.'
    }
} else {
    $winget = Get-Command winget -ErrorAction SilentlyContinue
    if ($winget) {
        Write-Host 'Chocolatey not found on PATH; falling back to winget (best-effort)...'
        $wingetIds = @('Kitware.CMake', 'Ninja-build.Ninja', 'Git.Git', 'ccache.ccache', 'ShiningLight.OpenSSL.Light', '7zip.7zip', 'cURL.cURL', 'GnuPG.GnuPG')
        foreach ($id in $wingetIds) {
            try {
                winget install --id $id --silent --accept-package-agreements --accept-source-agreements
                if ($LASTEXITCODE -ne 0) {
                    throw "winget install exited with code $LASTEXITCODE"
                }
                Write-Host "Installed: $id"
            } catch {
                Write-Host "Warning: winget install failed for '$id' (possibly an invalid package id); continuing with the remaining packages."
            }
        }
    } else {
        [Console]::Error.WriteLine('Neither Chocolatey nor winget is available on PATH.')
        [Console]::Error.WriteLine('Run the following commands from an elevated (admin) PowerShell:')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Option 1: install the Chocolatey bootstrapper, then:')
        [Console]::Error.WriteLine('  Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString(''https://community.chocolatey.org/install.ps1''))')
        [Console]::Error.WriteLine('  choco install -y --no-progress cmake ninja git ccache openssl 7zip zlib curl gnupg')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Option 2: install each package with winget:')
        [Console]::Error.WriteLine('  winget install --id Kitware.CMake --silent')
        [Console]::Error.WriteLine('  winget install --id Ninja-build.Ninja --silent')
        [Console]::Error.WriteLine('  winget install --id Git.Git --silent')
        [Console]::Error.WriteLine('  winget install --id ccache.ccache --silent')
        [Console]::Error.WriteLine('  winget install --id ShiningLight.OpenSSL.Light --silent')
        [Console]::Error.WriteLine('  winget install --id 7zip.7zip --silent')
        [Console]::Error.WriteLine('  winget install --id cURL.cURL --silent')
        [Console]::Error.WriteLine('  winget install --id GnuPG.GnuPG --silent')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Then re-run this script.')
        exit 1
    }
}

Write-Host 'Refreshing PATH for the current process...'
$machinePath = [Environment]::GetEnvironmentVariable('Path', 'Machine')
$userPath = [Environment]::GetEnvironmentVariable('Path', 'User')
$env:Path = ($machinePath + ';' + $userPath + ';C:\ProgramData\chocolatey\bin;C:\Program Files\CMake\bin;C:\Program Files\Ninja\bin')

foreach ($tool in @('curl', 'gpg')) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
        throw "$tool is required but was not found after package installation."
    }
}

$opensslCandidates = @(
    $env:OPENSSL_ROOT_DIR,
    "$env:ProgramFiles\OpenSSL-Win64",
    "$env:ProgramFiles\OpenSSL",
    "$env:ChocolateyToolsLocation\openssl"
)
$opensslCommand = Get-Command openssl -ErrorAction SilentlyContinue
if ($opensslCommand) {
    $opensslCandidates += Split-Path (Split-Path $opensslCommand.Source -Parent) -Parent
}

$opensslRoot = $opensslCandidates |
    Where-Object {
        $_ -and
        (Test-Path (Join-Path $_ 'include\openssl\ssl.h')) -and
        (Get-ChildItem -Path $_ -Filter 'libcrypto*.lib' -Recurse -ErrorAction SilentlyContinue)
    } |
    Select-Object -First 1

if (-not $opensslRoot) {
    throw 'OpenSSL installation was not found after package installation.'
}

$env:OPENSSL_ROOT_DIR = $opensslRoot
if ($env:GITHUB_ENV) {
    "OPENSSL_ROOT_DIR=$opensslRoot" | Out-File -FilePath $env:GITHUB_ENV -Encoding utf8 -Append
}
Write-Host "Using OpenSSL root: $opensslRoot"

Write-Host 'Discovering zlib development files...'
$zlibSearchRoots = @(
    $env:ZLIB_ROOT,
    "$env:ProgramFiles\zlib",
    "$env:ChocolateyToolsLocation\zlib",
    "$env:ChocolateyToolsLocation",
    "$env:ProgramData\chocolatey\lib"
) | Where-Object { $_ -and (Test-Path $_) }

$zlibHeader = $zlibSearchRoots | ForEach-Object {
    Get-ChildItem -Path $_ -Filter 'zlib.h' -Recurse -ErrorAction SilentlyContinue
} | Select-Object -First 1

if (-not $zlibHeader) {
    throw 'zlib development files (zlib.h) were not found after package installation.'
}

$zlibRoot = Split-Path (Split-Path $zlibHeader.FullName -Parent) -Parent
$zlibLib = Get-ChildItem -Path $zlibRoot -Filter 'zlib*.lib' -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $zlibLib) {
    throw "zlib import library (zlib*.lib) was not found under '$zlibRoot'."
}

$env:ZLIB_ROOT = $zlibRoot
if ($env:GITHUB_ENV) {
    "ZLIB_ROOT=$zlibRoot" | Out-File -FilePath $env:GITHUB_ENV -Encoding utf8 -Append
}
Write-Host "Using zlib root: $zlibRoot"

Write-Host 'Installed toolchain versions:'
foreach ($tool in @('cmake', 'ninja', 'git', 'ccache')) {
    if (Get-Command $tool -ErrorAction SilentlyContinue) {
        & $tool --version
    } else {
        Write-Host "$tool --version failed: $tool not found on PATH."
    }
}
