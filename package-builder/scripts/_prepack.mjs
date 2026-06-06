import node_fs from "node:fs"
import node_path from "node:path"

import SemverValid from "semver/functions/valid.js"

import ReactNativeUwsPackageJson from "../../package/package.json" with { type: "json" }

/**
 * @param {string} rootDir 
 */
export async function prepack(
	rootDir,
) {

	{
		// Replace placeholder version at the
		// /react-native-uws/package/src/_internal/const/version.ts

		const libraryVersion = SemverValid(ReactNativeUwsPackageJson.version)

		if(!libraryVersion) {
			throw new TypeError("Library version is not a valid semver")
		}

		const filePath = node_path.join(rootDir, "package", "src", "_internal", "const", "echo", "version.ts")

		if(!node_fs.existsSync(filePath)) {
			throw new Error(`${filePath} doesn't exist`)
		}

		let fileStr = node_fs.readFileSync(filePath).toString()
		fileStr = fileStr.replace(/(export const VERSION = )("X\.X\.X")/, `$1"${libraryVersion}"`)

		node_fs.writeFileSync(filePath, fileStr, { encoding: "utf8" })
	}

	{
		// Copy files

		const
			/**
			 * @type {{
			 * 	src: string,
			 * 	dest: string,
			 * }[]}
			 */
			filesToCopy =
				[
					{
						src: node_path.join(rootDir, "LICENSE"),
						dest: node_path.join(rootDir, "package", "LICENSE"),
					},
					{
						src: node_path.join(rootDir, "README.md"),
						dest: node_path.join(rootDir, "package", "README.md"),
					},
				]

		await Promise.all(
			filesToCopy.map(file => {
				return node_fs.promises.cp(
					file.src,
					file.dest,
				)
			}),
		)
	}

}
