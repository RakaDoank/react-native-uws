import NativeReactNativeUws from "../_internal/native-modules/NativeReactNativeUws"

import type {
	AppOptions,
	TemplatedApp,
} from "../types"

/**
 * Constructs a non-SSL app. An app is your starting point where you attach behavior to URL routes.
 * This is also where you listen and run your app, set any SSL options (in case of SSLApp) and the like.
 */
export function App(options?: AppOptions) : TemplatedApp {
	return NativeReactNativeUws.App(options) as unknown as TemplatedApp

	// const app = NativeReactNativeUws.App(options) as Record<string, unknown>

	// return {
	// 	__appID: app.id as string,

	// 	// @ts-expect-error Overloading methods
	// 	listen(...args) {
	// 		// Run the app
	// 		return this
	// 	},

	// 	// +++++ Router +++++

	// 	get(pattern, handler) {
	// 		return this
	// 	},

	// 	post(pattern, handler) {
	// 		return this
	// 	},

	// 	options(pattern, handler) {
	// 		return this
	// 	},

	// 	del(pattern, handler) {
	// 		return this
	// 	},

	// 	patch(pattern, handler) {
	// 		return this
	// 	},

	// 	put(pattern, handler) {
	// 		return this
	// 	},

	// 	head(pattern, handler) {
	// 		return this
	// 	},

	// 	connect(pattern, handler) {
	// 		return this
	// 	},

	// 	trace(pattern, handler) {
	// 		return this
	// 	},

	// 	any(pattern, handler) {
	// 		return this
	// 	},

	// 	// ----- Router -----

	// 	ws(pattern, behavior) {
	// 		return this
	// 	},

	// 	publish(topic, message, isBinary, compress) {
	// 		return this
	// 	},

	// 	numSubscribers(topic) {
	// 		return this
	// 	},

	// 	addServerName(hostname, options) {
	// 		return this
	// 	},

	// 	domain(domain) {
	// 		return this
	// 	},

	// 	removeServerName(hostname) {
	// 		return this
	// 	},

	// 	missingServerName(cb) {
	// 		return this
	// 	},

	// 	filter(cb) {
	// 		return this
	// 	},

	// 	close() {
	// 		return this
	// 	},

	// 	getDescriptor() {
	// 		return this
	// 	},

	// 	addChildAppDescriptor(descriptor) {
	// 		return this
	// 	},

	// 	removeChildAppDescriptor(descriptor) {
	// 		return this
	// 	},
	// }
}
