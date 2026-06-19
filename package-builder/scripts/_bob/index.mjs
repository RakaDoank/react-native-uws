import node_childProcess from "node:child_process"
import node_fs from "node:fs"
import node_path from "node:path"

import {
	prepack,
} from "./_prepack.mjs"

/**
 * @param {string} workspaceDir
 */
export async function bob(workspaceDir) {

	const
		libPath =
			node_path.join(workspaceDir, "package", "lib")

	await prepack(workspaceDir)

	if(node_fs.existsSync(libPath)) {
		node_fs.rmSync(
			libPath,
			{
				recursive: true,
				force: true,
			},
		)
	}

	node_childProcess.execSync(
		"npx bob build",
		{
			cwd: node_path.join(workspaceDir, "package-builder"),
			stdio: "inherit",
		},
	)

}
