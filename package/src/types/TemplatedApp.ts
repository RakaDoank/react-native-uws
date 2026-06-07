import type {
	ListenOptions,
} from "../modules/ListenOptions"

import type {
	AppDescriptor,
} from "./AppDescriptor"

import type {
	AppOptions,
} from "./AppOptions"

import type {
	HttpRequest,
} from "./HttpRequest"

import type {
	HttpResponse,
} from "./HttpResponse"

import type {
	RecognizedString,
} from "./RecognizedString"

import type {
	WebSocketBehavior,
} from "./WebSocketBehaviour"

import type {
	us_listen_socket,
} from "./us_listen_socket"

/**
 * Definitions of the `App` and `SSLApp`. This is entirely copied from uWebSockets.js
 */
export interface TemplatedApp {
	/**
	 * Listens to hostname & port and sets Listen Options. Callback hands either false or a listen socket.
	 */
	listen(
		host: RecognizedString,
		port: number,
		options: ListenOptions,
		cb: (listenSocket: us_listen_socket | false) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Listens to hostname & port. Callback hands either false or a listen socket.
	 */
	listen(
		host: RecognizedString,
		port: number,
		cb: (listenSocket: us_listen_socket | false) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Listens to port and sets Listen Options. Callback hands either false or a listen socket.
	 */
	listen(
		port: number,
		options: ListenOptions,
		cb: (listenSocket: us_listen_socket | false) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Listens to port. Callback hands either false or a listen socket.
	 */
	listen(
		port: number,
		cb: (listenSocket: us_listen_socket | false) => void | Promise<void>,
	) : TemplatedApp,

	// we can't provide this in Reaact Native
	/**
	 * Listens to unix socket. Callback hands either false or a listen socket.
	 */
	// listen_unix(
	// 	cb: (listenSocket: us_listen_socket) => void | Promise<void>,
	// 	path: RecognizedString,
	// ) : TemplatedApp,

	/**
	 * Registers an HTTP GET handler matching specified URL pattern.
	 */
	get(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP POST handler matching specified URL pattern.
	 */
	post(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP OPTIONS handler matching specified URL pattern.
	 */
	options(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP DELETE handler matching specified URL pattern.
	 */
	del(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP PATCH handler matching specified URL pattern.
	 */
	patch(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP PUT handler matching specified URL pattern.
	 */
	put(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP HEAD handler matching specified URL pattern.
	 */
	head(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP CONNECT handler matching specified URL pattern.
	 */
	connect(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP TRACE handler matching specified URL pattern.
	 */
	trace(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers an HTTP handler matching specified URL pattern on any HTTP method.
	 */
	any(
		pattern: RecognizedString,
		handler: (res: HttpResponse, req: HttpRequest) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Registers a handler matching specified URL pattern where WebSocket upgrade requests are caught.
	 */
	ws<UserData>(
		pattern: RecognizedString,
		behavior: WebSocketBehavior<UserData>,
	) : TemplatedApp,

	/**
	 * Publishes a message under topic, for all WebSockets under this app. See WebSocket.publish.
	 */
	publish(
		topic: RecognizedString,
		message: RecognizedString,
		isBinary?: boolean,
		compress?: boolean,
	) : boolean,

	/**
	 * Returns number of subscribers for this topic.
	 */
	numSubscribers(
		topic: RecognizedString,
	) : number,

	/**
	 * Adds a server name.
	 */
	addServerName(
		hostname: string,
		options: AppOptions,
	) : TemplatedApp,

	/**
	 * Browse to SNI domain. Used together with .get, .post and similar to attach routes under SNI domains.
	 */
	domain(
		domain: string,
	) : TemplatedApp,

	/**
	 * Removes a server name.
	 */
	removeServerName(
		hostname: string,
	) : TemplatedApp,

	/**
	 * Registers a synchronous callback on missing server names. See /examples/ServerName.js.
	 */
	missingServerName(
		cb: (hostname: string) => void,
	) : TemplatedApp,

	/**
	 * Attaches a "filter" function to track socket connections / disconnections.
	 */
	filter(
		cb: (res: HttpResponse, count: number) => void | Promise<void>,
	) : TemplatedApp,

	/**
	 * Closes all sockets including listen sockets. This will forcefully terminate all connections.
	 */
	close() : TemplatedApp,

	/**
	 * Returns the app descriptor for worker thread distribution.
	 */
	getDescriptor(): AppDescriptor,

	/**
	 * Add a child app descriptor for worker thread distribution.
	 */
	addChildAppDescriptor(
		descriptor: AppDescriptor,
	): void,

	/**
	 * Remove a child app descriptor.
	 */
	removeChildAppDescriptor(
		descriptor: AppDescriptor,
	): void,
}
