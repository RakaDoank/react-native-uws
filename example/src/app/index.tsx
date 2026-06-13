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

import * as uWS from "react-native-uws"

export default function Page() {

	CarbonStyleSheet.use()

	useEffect(() => {
		const app = uWS.App()

		app.get("/test", res => {
			res.end("Yeay");
		})

		app.get("/long-operation", async res => {
			let isAborted = false
			res.onAborted(() => {
				console.log("onAborted long operation")
				isAborted = true
			})

			await new Promise(resolver => {
				setTimeout(() => {
					resolver(true)
				}, 5000)
			})

			if(!isAborted) {
				res.end("long operation")
			}
		})

		app.get("/hola/:name/:type", (res, req) => {
			// res.onAborted(() => {
			// 	console.log("onAbort")
			// })
			// req.forEach((key, val) => {
			// 	console.log("header", key, val)
			// })

			// console.log("getCaseSensitiveMethod", req.getCaseSensitiveMethod())

			// console.log("getHeader", req.getHeader("connection"))

			// console.log("getMethod", req.getMethod())

			console.log("getParameter", req.getParameter(0), req.getParameter("name"))
			console.log("getParameter", req.getParameter(1), req.getParameter("type"))

			// This is only work in debugOptimized and release/production build
			// I don't know why this doesn't work on the basic debug build
			// console.log("getQuery", req.getQuery(), req.getQuery("foo"))

			// console.log("getUrl", req.getUrl(), req.getUrl().length)

			res.end("hello test")
		})

		app.get("/headers", (res, req) => {
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
			res.onDataV2((chunk, isLast) => {
				if(isLast) {
					res.writeHeader("content-type", "application/json")
					res.end(
						JSON.stringify({
							type: "onData",
							byteLength: chunk.byteLength,
						}),
					)
				}
			})
		})

		app.post("/ondatav2", (res) => {
			res.onDataV2((chunk, maxRemainingBodyLength) => {
				if(!maxRemainingBodyLength) {
					res.writeHeader("content-type", "application/json")
					res.end(
						JSON.stringify({
							type: "onDataV2",
							byteLength: chunk.byteLength,
						}),
					)
				}
			})
		})

		// TextDecoder is supported only for React Native 0.85 and latest
		app.post("/read-json-from-arraybuffer", res => {
			res.onFullData(chunk => {
				const textDecoder = new TextDecoder("utf-8")
				const text = textDecoder.decode(chunk)
				try {
					const json = JSON.parse(text) as unknown

					// send it back in JSON
					res.writeHeader("content-type", "application/json")
					res.end(
						JSON.stringify(json),
					)
				} catch {
					res.end("not valid data")
				}
			})
		})

		app.post("/read-json-from-text", res => {
			res.onFullDataText(chunk => {
				try {
					const json = JSON.parse(chunk) as unknown

					// send it back in JSON
					res.writeHeader("content-type", "application/json")
					res.end(
						JSON.stringify(json),
					)
				} catch {
					res.end("not valid data")
				}
			})
		})

		app.listen(5000, () => {
			console.log("Listen!")
		})

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
