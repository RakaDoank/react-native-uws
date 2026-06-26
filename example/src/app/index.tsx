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

import * as uWS from "uws-react-native"

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
				}, 3000)
			})

			if(!isAborted) {
				res.end("long operation")
			}
		})

		app.get("/data/:foo/:bar", (res, req) => {
			res.onAborted(() => {
				console.log("onAbort")
			})

			const forEachResult: Record<string, string> = {}

			req.forEach((key, val) => {
				forEachResult[key] = val
			})

			res.writeHeader("content-type", "application/json")
			res.end(
				JSON.stringify({
					forEach: forEachResult,
					getCaseSensitiveMethod: req.getCaseSensitiveMethod(),
					getHeader: req.getHeader("connection"),
					getParameter: `${req.getParameter(0)} - ${req.getParameter("foo")} | ${req.getParameter(1)} - ${req.getParameter("bar")}`,
					getMethod: req.getMethod(),
					getUrl: req.getUrl(),
					getQuery: req.getQuery() || null,
				}),
			)
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
			let isAborted = false
			res.onAborted(() => {
				isAborted = true
			})

			res.onData((chunk, isLast) => {
				if(isLast && !isAborted) {
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
			let isAborted = false
			res.onAborted(() => {
				isAborted = true
			})

			res.onDataV2((chunk, maxRemainingBodyLength) => {
				console.log("onDataV2", chunk.byteLength, maxRemainingBodyLength)
				if(!maxRemainingBodyLength && !isAborted) {
					res.writeHeader("content-type", "application/json")
					res.end(
						JSON.stringify({
							type: "onDataV2",
							byteLength: chunk.byteLength,
						}),
					)
				}

				// already stop collecting because the `maxBodySize` option
				// if(chunk.byteLength >= 1024 * 1024) {
				// 	res.writeHeader("content-type", "application/json")
				// 	res.end(
				// 		JSON.stringify({
				// 			type: "onDataV2",
				// 			byteLength: chunk.byteLength,
				// 			maxRemainingBodyLength,
				// 		}),
				// 	)
				// }
			})
		})

		app.post("/onfulldata", res => {
			let isAborted = false
			res.onAborted(() => {
				isAborted = true
			})

			res.onFullData(chunk => {
				console.log("onFullData", chunk.byteLength)
				if(!isAborted) {
					res.writeHeader("content-type", "application/json")
					res.end(
						JSON.stringify({
							type: "onFullData",
							byteLength: chunk.byteLength,
						}),
					)
				}
			})
		})

		// TextDecoder is supported only for React Native 0.85 and latest
		app.post("/read-json-from-arraybuffer", res => {
			let isAborted = false
			res.onAborted(() => {
				isAborted = true
			})

			if(!isAborted) {
				res.onFullData(chunk => {
					const textDecoder = new TextDecoder("utf-8")
					const text = textDecoder.decode(chunk)
					try {
						const json = JSON.parse(text) as unknown

						console.log("json", json)

						if(!isAborted) {
							// send it back in JSON
							res.writeHeader("content-type", "application/json")
							res.end(
								JSON.stringify(json),
							)
						}
					} catch {
						res.end("not valid data")
					}
				})
			}
		})

		app.post("/read-json-from-text", res => {
			let isAborted = false
			res.onAborted(() => {
				isAborted = true
			})

			if(!isAborted) {
				res.onFullDataText(body => {
					try {
						const json = JSON.parse(body) as unknown

						if(!isAborted) {
							// send it back in JSON
							res.writeHeader("content-type", "application/json")
							res.end(
								JSON.stringify(json),
							)
						}
					} catch {
						res.end("not valid data")
					}
				})
			}
		})

		app.post("/get-form-data", (res, req) => {
			const requestContentType = req.getHeader("content-type")

			console.log("/get-form-data requestContentType", requestContentType)
			if(requestContentType.startsWith("multipart/form-data")) {
				res.onFullData(body => {
					res.writeHeader("content-type", "application/json")
					console.log("/get-form-data fulldata", body.byteLength, requestContentType)

					try {
						const multipartField = uWS.getParts(body, requestContentType)

						if(multipartField?.length) {
							const textDecoder = new TextDecoder("utf-8")
							const text1Part = multipartField.find(p => p.name == "text1")

							let text1 = ""

							if(text1Part) {
								text1 = textDecoder.decode(text1Part.data)
							}

							res.end(
								JSON.stringify({
									fields: multipartField.map(field => {
										return {
											_byteLength: field.data.byteLength,
											name: field.name,
											filename: field.filename,
											type: field.type,
										}
									}),
									decodedFields: {
										text1,
									},
									success: true,
								}),
							)
						} else {
							throw new Error()
						}
					} catch(err) {
						console.log("/get-form-data error", err instanceof Error ? err.message : err)
						res.end(
							JSON.stringify({
								data: null,
								success: false,
							}),
						)
					}
				})
			} else {
				res.writeHeader("content-type", "application/json")
				res.end(
					JSON.stringify({
						data: null,
						success: false,
						message: "Header content-type is not multipart/form-data",
					}),
				)
			}
		})

		// iOS will fail to listen/run the server without explicit host 127.0.0.1
		app.listen("127.0.0.1", 5000, token => {
			if(token) {
				console.log("Listening at 5000")
			} else {
				console.log("Failed to listen")
			}
		})

		return () => {
			app.close()
		}
	}, [])

	return (
		<View
			style={ [
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
