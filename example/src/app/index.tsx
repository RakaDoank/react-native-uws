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

// import * as uws from "react-native-uws"

export default function Page() {

	CarbonStyleSheet.use()

	useEffect(() => {
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

		// return () => {
		// 	server.close();
		// }
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
