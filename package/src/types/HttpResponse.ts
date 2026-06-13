import type {
	RecognizedString,
} from "./RecognizedString"

// import type {
// 	us_socket_context_t,
// } from "./us_socket_context_t"

/**
 * An HttpResponse is valid until either onAborted callback or any of the .end/.tryEnd calls succeed. You may attach user data to this object.
 */
export interface HttpResponse {
	/**
	 * Immediately force closes the connection. Any onAborted callback will run.
	 */
	close() : HttpResponse,

	/**
	 * Corking a response is a performance improvement in both CPU and network, as you ready the IO system for writing multiple chunks at once.
	 * 
     * By default, you're corked in the immediately executing top portion of the route handler. In all other cases, such as when returning from
     * await, or when being called back from an async database request or anything that isn't directly executing in the route handler, you'll want
     * to cork before calling writeStatus, writeHeader or just write. Corking takes a callback in which you execute the writeHeader, writeStatus and
     * such calls, in one atomic IO operation. This is important, not only for TCP but definitely for TLS where each write would otherwise result
     * in one TLS block being sent off, each with one send syscall.
     *
     * Example usage:
     *
     * ```
     * res.cork(() => {
     *   res.writeStatus("200 OK").writeHeader("Some", "Value").write("Hello world!"),
     * }),
     * ```
	 */
	cork(
		cb: () => void,
	) : HttpResponse,

	// TODO
	// It is a custom method from uWebSockets.js.
	// I don't know how to implement this in React Native, since the JS call is always late than
	// our predefined `res->onDataV2` which already running and even may finished earlier.
	// /**
	//  * `collectBody` is a helper function making optimal use of the new onDataV2.
	//  * It allows efficient and easy collection of smallish HTTP request body data into RAM.
	//  * It accumulates all data chunks and calls handler with the complete body as an ArrayBuffer once all data has arrived.
	//  * If the total body size exceeds maxSize bytes, handler is called with null instead.
	//  */
	// collectBody(
	// 	maxSize: number,
	// 	handler: (fullBody: ArrayBuffer | null) => void,
	// ) : HttpResponse,

	/**
	 * Ends this response by copying the contents of body.
	 */
	end(
		body?: RecognizedString,
		closeConnection?: boolean,
	) : HttpResponse,

	/**
	 * Ends this response without a body.
	 */
	endWithoutBody(
		reportedContentLength?: number,
		closeConnection?: boolean,
	) : HttpResponse,

	// Would be implemented until ArrayBuffer supported by React Native
	// /**
	//  * Returns the remote IP address in binary format (4 or 16 bytes).
	//  */
	// getRemoteAddress() : ArrayBuffer,

	/**
	 * Returns the remote IP address as text.
	 */
	getRemoteAddressAsText() : string,

	/**
	 * Returns the remote port number.
	 */
	getRemotePort() : number,

	// TODO
	// Would be implemented until SSL has been implemented
	// /**
	//  * Returns the remote IP address in binary format (4 or 16 bytes), as reported by the PROXY Protocol v2 compatible proxy.
	//  */
	// getProxiedRemoteAddress() : ArrayBuffer,

	// TODO
	// Would be implemented until the SSL has been implemented
	// /**
	//  * Returns the remote IP address as text, as reported by the PROXY Protocol v2 compatible proxy.
	//  */
	// getProxiedRemoteAddressAsText() : string,

	// Would be implemented until we support the Proxy
	// /**
	//  * Returns the remote port number, as reported by the PROXY Protocol v2 compatible proxy.
	//  */
	// getProxiedRemotePort() : number,

	/**
	 * Returns the global byte write offset for this response. Use with onWritable.
	 */
	getWriteOffset() : number,

	/**
	 * Every HttpResponse MUST have an attached abort handler.
	 * If you do not respond to it immediately inside of the callback.
	 * Returning from an Http request handler without attaching
	 * (by calling onAborted) an abort handler is ill-use and will terminate.
     * When this event emits, the response has been aborted and may not be used.
	 */
	onAborted(
		handler: () => void,
	) : HttpResponse,

	/**
	 * Handler for reading HTTP request body data.
	 * Must be attached before performing any asynchronous operation, otherwise data may be lost.
	 * You MUST copy the data of chunk if isLast is not true. We Neuter ArrayBuffers on return, making them zero length.
	 * 
	 * ---
	 * 
	 * **Note**
	 * 
	 * If you have ever used uWebSockets or uWebSockets.js for Node.js,
	 * this handler/callback might be called lesser amount of times than normal uWebSockets due to predefined `onDataV2`
	 * that we already passed earlier in C++ side, and it might already finished before JS does an assignment or attachment to `onData`.
	 * 
	 * We have to pass the `onDataV2` lambda in C++ early because doing late assignment to the `onDataV2` handler will do nothing or our handler is never getting called. This is also due to uWebSockets run at different thread, and make any JS calls always late (asynchronously), and that is also by design to prevent Main/UI thread blocking.
	 */
	onData(
		handler: (
			chunk: ArrayBuffer,
			isLast: boolean,
		) => void,
	) : void,

	/**
	 * Similar to `onDataV2`, but it is for reading HTTP request body data in raw text.
	 * 
	 * This is an alternative method to read ArrayBuffer chunk in string because the TextDecoder is
	 * missing in React Native 0.84 and older.
	 */
	onDataText(
		handler: (
			chunk: string,
			maxRemainingBodyLength: bigint,
		) => void,
	) : void,

	/**
	 * Handler for reading HTTP request body data. V2.
	 * 
	 * Must be attached before performing any asynchronous operation, otherwise data may be lost.
	 * You MUST copy the data of chunk if maxRemainingBodyLength is not 0n. We Neuter ArrayBuffers on return, making them zero length.
	 * 
	 * maxRemainingBodyLength is the known maximum of the remaining body length. Can be used to preallocate a receive buffer.
	 * 
	 * ---
	 * 
	 * **Note**
	 * 
	 * If you have ever used uWebSockets or uWebSockets.js for Node.js,
	 * this handler/callback might be called lesser amount of times than normal uWebSockets due to predefined `onDataV2`
	 * that we already passed earlier in C++ side, and it might already finished before JS does an assignment or attachment to `onDataV2`.
	 * 
	 * We have to pass the `onDataV2` lambda in C++ early because doing late assignment to the `onDataV2` handler will do nothing or our handler is never getting called. This is also due to uWebSockets run at different thread, and make any JS calls always late (asynchronously), and that is also by design to prevent Main/UI thread blocking.
	 */
	onDataV2(
		handler: (
			chunk: ArrayBuffer,
			maxRemainingBodyLength: bigint,
		) => void,
	) : void,

	/**
	 * Handler for reading HTTP request body data only when all the body data has been retrieved,
	 * or the body size has reached the `maxBodySize` limit from your route handler options.
	 * A good case for a simple route handler that only needs to read complete body data.
	 * 
	 * Not like the `onData` and `onDataV2`, this is a good option to save a bit of operation cost
	 * because react-native-uws passes an ArrayBuffer from C++ to JS only once when it is finished.
	 * 
	 * This is an equivalent of `res.collectBody()` method from uWebSockets.js for Node.js,
	 * but to set max byte of body size, you have to set it through router options at the third arguments.
	 * 
	 * @example
	 * ```
	 * app.post("/create", (res, req) => {
	 *   let isAborted = false
	 *   res.onAborted(() => {
	 *     isAborted = true
	 *   })
	 * 
	 *   if(!isAborted) {
	 *     res.onFullData(chunk => {
	 *       if(!isAborted) {
	 *         res.end(`Body size is ${chunk.byteLength}`)
	 *       }
	 *     })
	 *   }
	 * }, {
	 *   maxBodySize: 1024 * 1024, // 1MB in byte
	 * })
	 * ```
	 */
	onFullData(
		handler: (
			chunk: ArrayBuffer,
		) => void,
	) : void,

	/**
	 * Similar to `onFullData`, but it is for reading HTTP request body data in raw text.
	 * 
	 * This is an alternative method to read ArrayBuffer chunk in string because the TextDecoder is
	 * missing in React Native 0.84 and older.
	 */
	onFullDataText(
		handler: (
			chunk: string,
		) => void,
	) : void,

	// TODO
	// Implement it later.
	// /**
	//  * Registers a handler for writable events. Continue failed write attempts in here.
	//  * You MUST return true for success, false for failure.
	//  * Writing nothing is always success, so by default you must return true.
	//  */
	// onWritable(
	// 	handler: (offset: number) => boolean,
	// ) : HttpResponse,

	/**
	 * Pause HTTP request body streaming (throttle).
     * Some buffered data may still be sent to onData.
	 */
	pause() : void,

	/**
	 * Resume HTTP request body streaming (unthrottle).
	 */
	resume() : void,

	/**
	 * Ends this response, or tries to, by streaming appropriately sized chunks of body. Use in conjunction with onWritable. Returns tuple [ok, hasResponded].
	 */
	tryEnd(
		fullBodyOrChunk: RecognizedString,
		totalSize: number,
	) : [boolean, boolean],

	// TODO
	// Implement this method later.
	// /**
	//  * Upgrades a HttpResponse to a WebSocket. See UpgradeAsync, UpgradeSync example files.
	//  * 
	//  * Currently, this library is only supported string for the user data.
	//  * As an object alternative, you can use `JSON.stringify` to this method,
	//  * and retrieve the user data back from ws.getUserData() with `JSON.parse`.
	//  */
	// upgrade/* <UserData extends object> */(
	// 	// userData: UserData,
	// 	userData: string,
	// 	secWebSocketKey: RecognizedString,
	// 	secWebSocketProtocol: RecognizedString,
	// 	secWebSocketExtensions: RecognizedString,
	// 	context: us_socket_context_t,
	// ) : void,

	/**
	 * Enters or continues chunked encoding mode. Writes part of the response. End with zero length write. Returns true if no backpressure was added.
	 */
	write(
		chunk: RecognizedString,
	) : boolean,

	/**
	 * Writes key and value to HTTP response.
     * See writeStatus and corking.
	 */
	writeHeader(
		key: RecognizedString,
		value: RecognizedString,
	) : HttpResponse,

	/**
	 * Writes the HTTP status message such as "200 OK".
     * This has to be called first in any response, otherwise
     * it will be called automatically with "200 OK".
     *
     * If you want to send custom headers in a WebSocket
     * upgrade response, you have to call writeStatus with
     * "101 Switching Protocols" before you call writeHeader,
     * otherwise your first call to writeHeader will call
     * writeStatus with "200 OK" and the upgrade will fail.
     *
     * As you can imagine, we format outgoing responses in a linear
     * buffer, not in a hash table. You can read about this in
     * the user manual under "corking".
	 */
	writeStatus(
		status: RecognizedString,
	) : HttpResponse,

	// TODO
	// the `upgrade` method needs to be implemented first,
	// but I also don't know how to attach arbitrary UserData to the HttpRequest object JSI C++.
	// Probably, I would tell user to get the data explicitly from the ws.getUserData() method
	// /**
	//  * Arbitrary user data may be attached to this object 
	//  */
	// [key: string]: unknown,
}
