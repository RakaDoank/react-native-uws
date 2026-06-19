#!/usr/bin/env node

import node_path from "node:path"

import * as argparse from "argparse"

const
	workspaceDir =
		node_path.join(import.meta.dirname, "..", ".."),

	argumentParser =
		new argparse.ArgumentParser(),

	commandParsers =
		argumentParser.add_subparsers({
			dest: "command",
			required: true,
		})

commandParsers.add_parser("bob", {
	help: "Bob the package",
})

commandParsers.add_parser("uws-mod", {
	help: "Clone uSockets and uWebSockets C++ files",
})

const
	args =
		argumentParser.parse_args()

try {
	if(args.command == "bob") {

		const { bob } = await import("./_bob/index.mjs")
		bob(workspaceDir)

	} else if(args.command == "uws-mod") {

		const { uws_mod } = await import("./_uws-mod.mjs")
		uws_mod(workspaceDir)

	} else {

		throw new Error("Unspecified command")
	}

} catch(err) {

	throw new Error(`/package-builder/scripts/bin.mjs :: ${err instanceof Error ? `[${err.name}] :: ${err.message}` : "Unknown error"}`)

}
