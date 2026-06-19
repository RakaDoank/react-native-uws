import node_childProcess from "node:child_process"
import node_fs from "node:fs"
import node_path from "node:path"
import node_stream from "node:stream/promises"

/**
 * @param {string} workspaceDir
 */
export async function uws_mod(workspaceDir) {

	if(
		process.platform != "linux" &&
		process.platform != "darwin" &&
		process.platform != "win32"
	) {
		throw new Error("uws-mod script is tested only for Linux, macOS, and Windows.")
	}

	const
		packageCppPath =
			node_path.join(workspaceDir, "package", "cpp"),

		uSockets =
			{
				gitTag: "v0.8.8",
				get url() {
					return `https://github.com/uNetworking/uSockets/archive/refs/tags/${this.gitTag}.zip`
				},
				zipFileDestination: node_path.join(packageCppPath, "uSockets.zip"),
				dirDestination: node_path.join(packageCppPath, "uSockets"),
			},

		uWebSockets =
			{
				gitTag: "v20.78.0",
				get url() {
					return `https://github.com/uNetworking/uWebSockets/archive/refs/tags/${this.gitTag}.zip`
				},
				zipFileDestination: node_path.join(packageCppPath, "uWebSockets.zip"),
				dirDestination: node_path.join(packageCppPath, "uWebSockets"),

				/**
				 * I need a secondary uWebSockets directory intentionally.
				 * When I want to include uWebSockets headers, I have to write with its scope directory path like this
				 * 
				 * ```c++
				 * #include "uWebSockets/App.h"
				 * ```
				 * 
				 * Meanwhile, I don't do this on uSockets to keep uWebSockets as original as how they write to include the uSockets header internally,
				 * ```c++
				 * #include "libusockets.h"
				 * ```
				 */
				get scopeDirDestination() {
					return node_path.join(this.dirDestination, "uWebSockets")
				},
			}

	if(node_fs.existsSync(uSockets.zipFileDestination)) {
		node_fs.rmSync(uSockets.zipFileDestination, { force: true })
	}

	if(node_fs.existsSync(uSockets.dirDestination)) {
		node_fs.rmSync(uSockets.dirDestination, { recursive: true, force: true })
	}

	if(node_fs.existsSync(uWebSockets.zipFileDestination)) {
		node_fs.rmSync(uWebSockets.zipFileDestination, { force: true })
	}

	if(node_fs.existsSync(uWebSockets.dirDestination)) {
		node_fs.rmSync(uWebSockets.dirDestination, { recursive: true, force: true })
	}

	await Promise
		.all([
			fetch(uSockets.url),
			fetch(uWebSockets.url),
		])
		.then(async ([us, uws]) => {
			if(!us.body) {
				throw new Error("Danger! We can't found uSockets source files at " + uSockets.url)
			}

			if(!uws.body) {
				throw new Error("Danger! We can't found uWebSockets source files at " + uWebSockets.url)
			}

			const
				usFileStream =
					node_fs.createWriteStream(uSockets.zipFileDestination),

				uwsFileStream =
					node_fs.createWriteStream(uWebSockets.zipFileDestination)

			await Promise.all([
				node_stream.pipeline(us.body, usFileStream),
				node_stream.pipeline(uws.body, uwsFileStream),
			])

			{
				if(process.platform == "linux" || process.platform == "darwin") {
					node_childProcess.execSync(
						`unzip ${uSockets.zipFileDestination} -d ${packageCppPath}`,
					)
				} else {
					node_childProcess.execSync(
						`powershell -Command "Expand-Archive -Path ${node_path.win32.normalize(uSockets.zipFileDestination)} -DestinationPath ${node_path.win32.normalize(packageCppPath)}"`,
					)
				}

				const unzippedDirectory = node_path.join(packageCppPath, `uSockets-${uSockets.gitTag.replace("v", "")}`)

				node_fs.cpSync(
					node_path.join(unzippedDirectory, "src"),
					uSockets.dirDestination,
					{
						recursive: true,
						force: true,
					},
				)
				await Promise.all([
					node_fs.promises.rm(uSockets.zipFileDestination, { force: true }),
					node_fs.promises.rm(unzippedDirectory, { recursive: true, force: true }),
				])
			}

			{
				if(process.platform == "linux" || process.platform == "darwin") {
					node_childProcess.execSync(
						`unzip ${uWebSockets.zipFileDestination} -d ${packageCppPath}`,
					)
				} else {
					node_childProcess.execSync(
						`powershell -Command "Expand-Archive -Path ${node_path.win32.normalize(uWebSockets.zipFileDestination)} -DestinationPath ${node_path.win32.normalize(packageCppPath)}"`,
					)
				}

				// create the first uWebSockets directory
				node_fs.mkdirSync(uWebSockets.dirDestination)

				const unzippedDirectory = node_path.join(packageCppPath, `uWebSockets-${uWebSockets.gitTag.replace("v", "")}`)

				// uWebSockets/uWebSockets/XXX.h
				node_fs.cpSync(
					node_path.join(unzippedDirectory, "src"),
					uWebSockets.scopeDirDestination,
					{
						recursive: true,
						force: true,
					},
				)

				await Promise.all([
					node_fs.promises.rm(uWebSockets.zipFileDestination, { force: true }),
					node_fs.promises.rm(unzippedDirectory, { recursive: true, force: true }),
				])
			}
		})

}
