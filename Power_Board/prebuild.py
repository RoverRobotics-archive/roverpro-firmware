#! python3.7

import subprocess
import datetime
import re
import sys

targetpath = sys.argv[1]
now = datetime.datetime.now()
print('Writing generated file to '+ targetpath)

version = subprocess.check_output(['git', 'describe', '--tags']).decode().strip()
with open(targetpath, 'w') as f:
    m = re.match(r'(\d+)\.(\d+)\.(\d+)-?(.*)', version)
    major, minor, patch, prerelease = m.groups()
    prerelease_c_str = '"' + prerelease.replace('\\', '_').replace('"', '_') + '"'

    f.write(f"""\
/// @file
/// Firmware version information.
/// Generated at time of build by {__file__}
#ifndef VERSION_GENERATED_H
#define VERSION_GENERATED_H

#include <stdint.h>

#define RELEASE_VERSION_NAME "{version}"
/// Semantic version as per semver.org
typedef struct {{
	/// serial number for incompatible API changes
    unsigned major;
    /// serial number for feature releases
    unsigned minor;
    /// serial number for bug fixes
    unsigned patch;
    /// string value to track ad-hoc changes and non-released versions
    char prerelease[];
}} ReleaseVersion;
#define RELEASE_VERSION {{{major}, {minor}, {patch}, {prerelease_c_str}}}
#define RELEASE_VERSION_FLAT {int(major) * 10000 + int(minor) * 100 + int(patch)}
#define BUILD_DATE "{now.date().isoformat()}"
#define BUILD_TIME "{now.time().replace(microsecond=0).isoformat()}"
#endif
""")
