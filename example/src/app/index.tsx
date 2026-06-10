import {
	useEffect,
} from "react"

import {
	View,
} from "react-native"

import {
	CarbonStyleSheet,
	Text,
} from "@audira/carbon-react-native"

import * as uws from "react-native-uws"

export default function Page() {

	CarbonStyleSheet.use()

	useEffect(() => {
		const app = uws.App()

		app.get("/long-operation", async (res) => {
			// We already assigned `onAborted`
			// internally in C++
			// res.onAborted(() => {
			// 	console.log("onAborted")
			// })

			await new Promise(resolver => {
				setTimeout(() => {
					resolver(true)
				}, 2000)
			})

			res.end("long operation")
		})

		app.get("/hola/:name", (res, req) => {
			res.onAborted(() => {
				console.log("onAbort")
			})
			req.forEach((key, val) => {
				console.log("header", key, val)
			})

			console.log("getCaseSensitiveMethod", req.getCaseSensitiveMethod())

			// why it's empty
			console.log("getHeader", req.getHeader("connection"))

			console.log("getMethod", req.getMethod())

			console.log("getParameter", req.getParameter(0))

			// This is only work in debugOptimized and release/production build
			// I don't know why this doesn't work on the basic debug build
			// console.log("getQuery", req.getQuery("foo"))

			console.log("getUrl", req.getUrl(), req.getUrl().length)

			res.end("hello test")
		})

		app.get("/testing", (res, req) => {
			res.write("<html><body><h2>Hello, your headers are:</h2><ul>")

			req.forEach((k, v) => {
				res.write("<li>")
				res.write(k)
				res.write(" = ")
				res.write(v)
				res.write("</li>")
			})

			res.end("</ul></body></html>");
		})

		app.post("/ondata", res => {
			console.log("/ondata")
			res.onData((chunk, isLast) => {
				console.log("ondata", chunk.length, isLast)
				if(isLast) {
					res.end("finished")
				}
			})
		})

		app.post("/ondatav2", (res) => {
			console.log("/ondatav2")
			res.onDataV2((chunk, maxRemainingBodyLength) => {
				console.log("onDataV2", chunk.byteLength, maxRemainingBodyLength)

				if(!maxRemainingBodyLength) {
					res.end("finish")
				}
			})
		})

		app.listen(5000, () => {
			console.log("Listen!")
		})

		// const server = new Echo.Http.Server()

		// server.route("/api/route", (request) => {
		// 	// return Echo.Http.Response.json({
		// 	// 	bool: true,
		// 	// 	number: Math.random(),
		// 	// 	string: "Ich komme aus Deutschland",
		// 	// 	array: [{
		// 	// 		bool: true,
		// 	// 		number: Math.random(),
		// 	// 		string: "Ich komme aus Osterreich",
		// 	// 	}, {
		// 	// 		bool: true,
		// 	// 		number: Math.random(),
		// 	// 		string: "Ich komme aus Frankreich",
		// 	// 	}],
		// 	// })

		// 	// mock long run
		// 	return new Promise<Echo.Http.Response>(resolve => {
		// 		setTimeout(() => {
		// 			resolve(
		// 				Echo.Http.Response.json({
		// 					bool: true,
		// 					number: Math.random(),
		// 					string: "Ich komme aus Deutschland",
		// 					array: [{
		// 						bool: true,
		// 						number: Math.random(),
		// 						string: "Ich komme aus Osterreich",
		// 					}, {
		// 						bool: true,
		// 						number: Math.random(),
		// 						string: "Ich komme aus Frankreich",
		// 					}],
		// 				}),
		// 			)
		// 		}, 6000)
		// 	})
		// })

		// server.route("/hello/:name", () => {
		// 	return Echo.Http.Response.json({ hello: "world" })
		// })

		// server.route("/test", () => {
		// 	return Echo.Http.Response.json({ test: "test" })
		// })

		// server.listen(5000, () => {
		// 	console.log("ONSTART")
		// })

		return () => {
			app.close()
		}
	}, [])

	// useEffect(() => {
	// 	const server = new Echo.Http.Server()

	// 	server.get(
	// 		"/api/hello/world",
	// 		() => {
	// 			return Echo.Http.Response.json(
	// 				{
	// 					string: "Hello World",
	// 					number: Math.random(),
	// 					boolean: false,
	// 					array: [
	// 						{
	// 							message: "Ich komme aus Osterreich",
	// 							null: null,
	// 						},
	// 						{
	// 							number: -1,
	// 							boolean: true,
	// 						},
	// 					],
	// 					null: null,
	// 					undefined: undefined,
	// 				},
	// 			)
	// 		},
	// 	)

	// 	server.get(
	// 		"/api/nocontent",
	// 		request => {
	// 			console.log(request.url.pathname)
	// 			return new Echo.Http.Response(null)
	// 		},
	// 	)

	// 	server.post(
	// 		"/api/json",
	// 		async request => {
	// 			try {
	// 				const json = await request.json() as unknown as Record<string, string>
	// 				return Echo.Http.Response.json(
	// 					{
	// 						foo: "bar",
	// 						data: json,
	// 					},
	// 				)
	// 			} catch(err) {
	// 				return Echo.Http.Response.json(
	// 					{
	// 						foo: null,
	// 						data: null,
	// 						error: err instanceof Error ? {
	// 							message: err.message,
	// 						} : undefined,
	// 					},
	// 				)
	// 			}
	// 		},
	// 	)

	// 	server.post(
	// 		"/api/formdata",
	// 		async request => {
	// 			try {
	// 				const formData = await request.formData()
	// 				return Echo.Http.Response.json(
	// 					{
	// 						yeay: true,
	// 						file: formData.get("file1"),
	// 					},
	// 				)
	// 			} catch(err) {
	// 				return Echo.Http.Response.json(
	// 					{
	// 						yeay: false,
	// 						error: err instanceof Error ? {
	// 							message: err.message,
	// 						} : undefined,
	// 					},
	// 				)
	// 			}
	// 		},
	// 	)

	// 	server.put(
	// 		"/api/put",
	// 		async request => {
	// 			try {
	// 				const headersObj: Record<string, string> = {}
	// 				request.headers.forEach((value, key) => {
	// 					headersObj[key] = value
	// 				})

	// 				return Echo.Http.Response.json({
	// 					put: true,
	// 					headers: headersObj,
	// 					data: await request.json() as unknown,
	// 				})
	// 			} catch(err) {
	// 				return Echo.Http.Response.json(
	// 					{
	// 						yeay: false,
	// 						error: err instanceof Error ? {
	// 							message: err.message,
	// 						} : undefined,
	// 					},
	// 				)
	// 			}
	// 		},
	// 	)

	// 	server.delete(
	// 		"/api/delete",
	// 		() => {
	// 			return new Echo.Http.Response(
	// 				null,
	// 				{
	// 					status: 204,
	// 				},
	// 			)
	// 		},
	// 	)

	// 	server.listen(
	// 		4040,
	// 		function() {
	// 			console.log("onListened")
	// 		},
	// 	)

	// 	return () => {
	// 		server.close()
	// 	}
	// }, [])

	return (
		<View
			style={ [
				{ backgroundColor: "red" },
				CarbonStyleSheet.g.py_09,
				CarbonStyleSheet.g.px_05,
			] }
		>
			<Text
				style={ [
					carbonStyleSheet.text,
				] }
			>
				Hit the server from your PC to the Android Emulator (through ADB port forwarding) or to an Android device IP in the same network.
			</Text>
		</View>
	)

}

const
	carbonStyleSheet =
		CarbonStyleSheet.create({
			text: {
				color: CarbonStyleSheet.color.text_primary,
			},
		})
