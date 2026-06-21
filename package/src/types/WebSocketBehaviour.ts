// import type {
// 	CompressOptions,
// } from "../modules/CompressOptions"

// import type {
// 	HttpRequest,
// } from "./HttpRequest"

// import type {
// 	HttpResponse,
// } from "./HttpResponse"

// import type {
// 	WebSocket,
// } from "./WebSocket"

// import type {
// 	us_socket_context_t,
// } from "./us_socket_context_t"

// /**
//  * A structure holding settings and handlers for a WebSocket URL route handler.
//  */
// export interface WebSocketBehavior/* <UserData> */ {
// 	/**
// 	 * Maximum length of received message. If a client tries to send you a message larger than this, the connection is immediately closed. Defaults to 16 * 1024.
// 	 */
// 	maxPayloadLength?: number,

// 	/**
// 	 * Whether or not we should automatically close the socket when a message is dropped due to backpressure. Defaults to false.
// 	 */
// 	closeOnBackpressureLimit?: boolean,

// 	/**
// 	 * Maximum number of minutes a WebSocket may be connected before being closed by the server. 0 disables the feature. Valid values are 0 and 1-239.
// 	 */
// 	maxLifetime?: number,

// 	/**
// 	 * Maximum amount of seconds that may pass without sending or getting a message. Connection is closed if this timeout passes. Resolution (granularity) for timeouts are typically 4 seconds, rounded to closest.
//      * Disable by using 0. Defaults to 120.
// 	 */
// 	idleTimeout?: number,

// 	/**
// 	 * What permessage-deflate compression to use. uWS.DISABLED, uWS.SHARED_COMPRESSOR or any of the uWS.DEDICATED_COMPRESSOR_xxxKB. Defaults to uWS.DISABLED.
// 	 */
// 	compression?: CompressOptions,

// 	/**
// 	 * Maximum length of allowed backpressure per socket when publishing or sending messages. Slow receivers with too high backpressure will be skipped until they catch up or timeout. Defaults to 64 * 1024.
// 	 */
// 	maxBackpressure?: number,

// 	/**
// 	 * Whether or not we should automatically send pings to uphold a stable connection given whatever idleTimeout.
// 	 */
// 	sendPingsAutomatically?: boolean,

// 	/**
// 	 * Upgrade handler used to intercept HTTP upgrade requests and potentially upgrade to WebSocket.
//      * See UpgradeAsync and UpgradeSync example files.
// 	 */
// 	upgrade?: (
// 		res: HttpResponse,
// 		req: HttpRequest,
// 		context: us_socket_context_t,
// 	) => void | Promise<void>,

// 	/**
// 	 * Handler for new WebSocket connection. WebSocket is valid from open to close, no errors.
// 	 */
// 	open?: (
// 		ws: WebSocket/* <UserData> */,
// 	) => void | Promise<void>,

// 	/**
// 	 * Handler for a WebSocket message. Messages are given as ArrayBuffer no matter if they are binary or not. Given ArrayBuffer is valid during the lifetime of this callback (until first await or return) and will be neutered.
// 	 */
// 	message?: (
// 		ws: WebSocket/* <UserData> */,
// 		message: ArrayBuffer,
// 		isBinary: boolean,
// 	) => void | Promise<void>,

// 	/**
// 	 * Handler for a dropped WebSocket message. Messages can be dropped due to specified backpressure settings. Messages are given as ArrayBuffer no matter if they are binary or not. Given ArrayBuffer is valid during the lifetime of this callback (until first await or return) and will be neutered.
// 	 */
// 	dropped?: (
// 		ws: WebSocket/* <UserData> */,
// 		message: ArrayBuffer,
// 		isBinary: boolean,
// 	) => void | Promise<void>,

// 	/**
// 	 * Handler for when WebSocket backpressure drains. Check ws.getBufferedAmount(). Use this to guide / drive your backpressure throttling.
// 	 */
// 	drain?: (
// 		ws: WebSocket/* <UserData> */,
// 	) => void,

// 	/**
// 	 * Handler for close event, no matter if error, timeout or graceful close. You may not use WebSocket after this event. Do not send on this WebSocket from within here, it is closed.
// 	 */
// 	close?: (
// 		ws: WebSocket/* <UserData> */,
// 		code: number,
// 		message: ArrayBuffer,
// 	) => void,

// 	/**
// 	 * Handler for received ping control message. You do not need to handle this, pong messages are automatically sent as per the standard.
// 	 */
// 	ping?: (
// 		ws: WebSocket/* <UserData> */,
// 		message: ArrayBuffer,
// 	) => void,

// 	/**
// 	 * Handler for received pong control message.
// 	 */
// 	pong?: (
// 		ws: WebSocket/* <UserData> */,
// 		message: ArrayBuffer,
// 	) => void,

// 	/**
// 	 * Handler for subscription changes.
// 	 */
// 	subscription?: (
// 		ws: WebSocket/* <UserData> */,
// 		topic: ArrayBuffer,
// 		newCount: number,
// 		oldCount: number,
// 	) => void,
// }
