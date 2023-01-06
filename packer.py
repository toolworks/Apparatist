#!/usr/bin/env python3
import shutil
import os
import json
import argparse
import glob
import logging
import semver
from enum import Enum

class PackageType(Enum):
    GAME = 1
    ENGINE = 2
    def __str__(self):
        if self.value == PackageType.GAME.value:
            return "game"
        else:
            return "engine"

PACKAGE_SITE = "Package.Site"
CONFIG_FILENAME = "PackerConfig.json"
plugin_name = os.path.basename(os.getcwd())

try:
    plugin_file = glob.glob("*.uplugin")[0]
except IndexError:
    logging.error("No .uplugin file was found in: " + os.getcwd())
    raise

# The source plugin's version with only major, minor, hotfix parts.
main_plugin_version = "Unknown"
# The plugin's version within the filename (without the hotfix, for easier upgrades):
short_plugin_version = "Unknown"
engine_version = "Unknown"
engine_version = []

try:
    with open(CONFIG_FILENAME) as json_file:
        config = json.load(json_file)
        if 'EngineVersions' in config:
            engine_versions = config['EngineVersions']
except:
    pass

with open(plugin_file) as json_file:
    data = json.load(json_file)
    if 'VersionName' in data:
        main_plugin_version = data['VersionName']
    if 'EngineVersion' in data:
        engine_version = data['EngineVersion']

if len(engine_versions) == 0:
    engine_versions = [engine_version]

try:
    main_plugin_version = semver.VersionInfo.parse(main_plugin_version)
    short_plugin_version = f"{main_plugin_version.major}.{main_plugin_version.minor}"
    main_plugin_version = f"{main_plugin_version.major}.{main_plugin_version.minor}.{main_plugin_version.patch}"
except:
    pass

try:
    engine_version = semver.VersionInfo.parse(engine_version)
    engine_version = f"{engine_version.major}.{engine_version.minor}"
except:
    pass

COMPONENTS = ["Config", "Docs", "Source", "Resources"]

parser = argparse.ArgumentParser(description='Pack the plugin for distribution.')
parser.add_argument('--engine', '-e', action='store_const', dest='type',
                    const=PackageType.ENGINE, default=PackageType.GAME,
                    help='Pack Engine-level version of the plugin.')
parser.add_argument('--unreal-engine', '-ue', dest='ue',
                    default=engine_version,
                    help='A version of Unreal Engine to package for.')

args = parser.parse_args()

def pack(engine_version):
    try:
        engine_version = semver.VersionInfo.parse(str(engine_version))
    except ValueError:
        try:
            engine_version = semver.VersionInfo.parse(str(engine_version) + ".0")
        except:
            pass

    if type(engine_version) is semver.VersionInfo:
        short_engine_version = f"{engine_version.major}.{engine_version.minor}"
    else:
        short_engine_version = str(engine_version)

    try:
        shutil.rmtree(PACKAGE_SITE)
    except:
        pass

    try:
        os.mkdir(PACKAGE_SITE)
    except:
        pass

    for comp in COMPONENTS:
        dst_path = os.path.join(PACKAGE_SITE, comp)
        try:
            if os.path.isfile(comp):
                shutil.copy(comp, dst_path)
            else:
                shutil.copytree(comp, dst_path)
        except:
            pass

    full_plugin_version = f"{main_plugin_version}+ue.{short_engine_version}.{args.type}"
    # The file-level plugin version is omitting the patch number:
    file_plugin_version = f"{short_plugin_version}+ue.{short_engine_version}.{args.type}"
    with open(plugin_file) as json_file:
        data = json.load(json_file)

    data['VersionName'] = full_plugin_version
    data['EngineVersion'] = str(engine_version)

    # Remove the deprecated platform...
    if engine_version.major >= 5:
        for module in data['Modules']:
            platforms = module['WhitelistPlatforms']
            if "Win32" in platforms:
                platforms.remove("Win32")

    for module in data['Modules']:
        platforms = module['WhitelistPlatforms']
        if (module["Type"] == "Runtime") or (module["Type"] == "UncookedOnly"):
            if "IOS" not in platforms:
                platforms.append("IOS")
            if "Switch" not in platforms:
                platforms.append("Switch")
            if "PS4" not in platforms:
                platforms.append("PS4")
            if "XboxOne" not in platforms:
                platforms.append("XboxOne")

    # Change the configuration file name:
    if args.type == PackageType.ENGINE:
        src = os.path.join(PACKAGE_SITE, "Config", f"Default{plugin_name}.ini")
        if os.path.isfile(src):
            dst = os.path.join(PACKAGE_SITE, "Config", f"Base{plugin_name}.ini")
            shutil.move(src, dst)

    site_plugin_file = os.path.join(PACKAGE_SITE, plugin_file)
    with open(site_plugin_file, 'w') as outfile:
        json.dump(data, outfile, indent='\t')

    shutil.make_archive(f"{plugin_name}-{file_plugin_version}", 'zip', PACKAGE_SITE)
    shutil.rmtree(PACKAGE_SITE)

for ev in engine_versions:
    pack(ev)
