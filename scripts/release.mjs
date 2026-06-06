import node_childProcess from "node:child_process"
import node_path from "node:path"

import SemverPrelease from "semver/functions/prerelease.js"

import ReactNativeEchoPackageJson from "../package/package.json" with { type: "json" }

const
	rootDir =
		node_path.join(import.meta.dirname, ".."),

	libraryVersionPrerelease =
		SemverPrelease(ReactNativeEchoPackageJson.version)

let publishCommand = "pnpm publish --filter react-native-uws --access public --no-git-checks"

if(typeof libraryVersionPrerelease?.[0] == "string") {
	publishCommand += ` --tag ${libraryVersionPrerelease[0]}`
}

node_childProcess.execSync(
	publishCommand,
	{
		cwd: rootDir,
		stdio: "inherit",
	},
)
