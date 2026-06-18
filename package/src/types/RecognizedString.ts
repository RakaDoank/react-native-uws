/**
 * Recognized string types, things C++ can read and understand as strings.
 * "String" does not have to mean "text", it can also be "binary".
 * 
 * Ironically, JavaScript strings are the least performant of all options, to pass or receive to/from C++.
 * This because we expect UTF-8. JavaScript strings are Latin-1 or UTF-16 internally meaning extra copies and reinterpretation are required.
 * That's why all events pass data by ArrayBuffer and not JavaScript strings, as they allow zero-copy data passing.
 */
export type RecognizedString =
	| string
	| ArrayBuffer
