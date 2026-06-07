import type {
	RecognizedString,
} from "./RecognizedString"

/**
 * A WebSocket connection that is valid from open to close event.
 * Read more about this in the user manual.
 */
export interface WebSocket<UserData> {
	/**
	 * Sends a message. Returns 1 for success, 2 for dropped due to backpressure limit, and 0 for built up backpressure that will drain over time. You can check backpressure before or after sending by calling getBufferedAmount().
     *
     * Make sure you properly understand the concept of backpressure. Check the backpressure example file.
     */
	send(
		message: RecognizedString,
		isBinary?: boolean,
		compress?: boolean,
	) : number,

	/**
	 * Returns the bytes buffered in backpressure. This is similar to the bufferedAmount property in the browser counterpart.
     * Check backpressure example.
     */
	getBufferedAmount() : number,

	/**
	 * Gracefully closes this WebSocket. Immediately calls the close handler.
     * A WebSocket close message is sent with code and shortMessage.
     */
	end(
		code?: number,
		shortMessage?: RecognizedString,
	) : void,

	/**
	 * Forcefully closes this WebSocket. Immediately calls the close handler.
     * No WebSocket close message is sent.
     */
	close() : void,

	/**
	 * Sends a ping control message. Returns sendStatus similar to WebSocket.send (regarding backpressure). This helper function correlates to WebSocket::send(message, uWS::OpCode::PING, ...) in C++.
	 */
	ping(
		message?: RecognizedString,
	) : number,

	/** Subscribe to a topic. */
	subscribe(
		topic: RecognizedString,
	) : boolean,

	/** Unsubscribe from a topic. Returns true on success, if the WebSocket was subscribed. */
	unsubscribe(
		topic: RecognizedString,
	) : boolean,

	/** Returns whether this websocket is subscribed to topic. */
	isSubscribed(
		topic: RecognizedString,
	) : boolean,

	/**
	 * Returns a list of topics this websocket is subscribed to.
	 */
	getTopics() : string[],

	/**
	 * Publish a message under topic. Backpressure is managed according to maxBackpressure, closeOnBackpressureLimit settings.
     * Order is guaranteed since v20.
     */
	publish(
		topic: RecognizedString,
		message: RecognizedString,
		isBinary?: boolean,
		compress?: boolean,
	) : boolean,

	/**
	 * See `HttpResponse.cork`. Takes a function in which the socket is corked (packing many sends into one single syscall/SSL block)
	 */
	cork(
		cb: () => void,
	) : WebSocket<UserData>,

	/**
	 * Returns the remote IP address. Note that the returned IP is binary, not text.
     *
     * IPv4 is 4 byte long and can be converted to text by printing every byte as a digit between 0 and 255.
     * IPv6 is 16 byte long and can be converted to text in similar ways, but you typically print digits in HEX.
     *
     * See getRemoteAddressAsText() for a text version.
     */
	getRemoteAddress() : ArrayBuffer,

	/**
	 * Returns the remote IP address as text. See RecognizedString.
	 */
	getRemoteAddressAsText() : ArrayBuffer,

	/**
	 * Returns the remote port number.
	 */
	getRemotePort() : number,

	/**
	 * Returns the UserData object.
	 */
	getUserData() : UserData,

	/**
	 * Sends the first fragment of a fragmented message. Use for sending large messages in chunks.
     * Returns 1 for success, 2 for dropped due to backpressure limit, and 0 for built up backpressure.
     */
	sendFirstFragment(
		message: RecognizedString,
		isBinary?: boolean,
		compress?: boolean,
	) : number,

	/**
	 * Sends a middle fragment of a fragmented message.
     * Returns 1 for success, 2 for dropped due to backpressure limit, and 0 for built up backpressure.
     */
	sendFragment(
		message: RecognizedString,
		compress?: boolean,
	) : number,

	/**
	 * Sends the last fragment of a fragmented message.
     * Returns 1 for success, 2 for dropped due to backpressure limit, and 0 for built up backpressure.
     */
	sendLastFragment(
		message: RecognizedString,
		compress?: boolean,
	) : number,
}
