#!/usr/bin/env node

import node_childProcess from "node:child_process"
import node_fs from "node:fs"
import node_path from "node:path"

import {
	prepack,
} from "./_prepack.mjs"

const
	rootDir =
		node_path.join(import.meta.dirname, "..", "..")

try {
	await prepack(rootDir)

	const libPath = node_path.join(rootDir, "package", "lib")

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
			cwd: node_path.join(rootDir, "package-builder"),
			stdio: "inherit",
		},
	)

} catch(err) {

	throw new Error(`/package-builder/scripts/bin.mjs :: ${err instanceof Error ? `[${err.name}] :: ${err.message}` : "Unknown error"}`)

}
