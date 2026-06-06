import NativeReactNativeEcho from "../_internal/native-modules/NativeReactNativeUws"

/**
 * WebSocket compression options. Combine any compressor with any decompressor using bitwise OR.
 */
export enum CompressOptions {
	/**
	 * No compression (always a good idea if you operate using an efficient binary protocol)
	 */
	DISABLED = -1,

	/**
	 * Zero memory overhead compression.
	 */
	SHARED_COMPRESSOR = -1,

	/**
	 * Zero memory overhead decompression.
	 */
	SHARED_DECOMPRESSOR = -1,

	/**
	 * Sliding dedicated compress window, requires 3KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_3KB = -1,

	/**
	 * Sliding dedicated compress window, requires 4KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_4KB = -1,

	/**
	 * Sliding dedicated compress window, requires 8KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_8KB = -1,

	/**
	 * Sliding dedicated compress window, requires 16KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_16KB = -1,

	/**
	 * Sliding dedicated compress window, requires 32KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_32KB = -1,

	/**
	 * Sliding dedicated compress window, requires 64KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_64KB = -1,

	/**
	 * Sliding dedicated compress window, requires 128KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_128KB = -1,

	/**
	 * Sliding dedicated compress window, requires 256KB of memory per socket
	 */
	DEDICATED_COMPRESSOR_256KB = -1,

	/**
	 * Sliding dedicated decompress window, requires 32KB of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR_32KB = -1,

	/**
	 * Sliding dedicated decompress window, requires 16KB of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR_16KB = -1,

	/**
	 * Sliding dedicated decompress window, requires 8KB of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR_8KB = -1,

	/**
	 * Sliding dedicated decompress window, requires 4KB of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR_4KB = -1,

	/**
	 * Sliding dedicated decompress window, requires 2KB of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR_2KB = -1,

	/**
	 * Sliding dedicated decompress window, requires 1KB of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR_1KB = -1,

	/**
	 * Sliding dedicated decompress window, requires 512B of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR_512B = -1,

	/**
	 * Sliding dedicated decompress window, requires 32KB of memory per socket (plus about 23KB)
	 */
	DEDICATED_DECOMPRESSOR = -1,
}
