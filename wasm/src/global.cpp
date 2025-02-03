/**
 * Copyright (c) 2017-2024 Paul-Louis Ageneau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "global.hpp"

namespace rtc {

void InitLogger([[maybe_unused]] LogLevel level, [[maybe_unused]] LogCallback callback) {
	// Dummy
}

void Preload() {
	// Dummy
}

std::shared_future<void> Cleanup() {
	// Dummy
	std::promise<void> p;
	p.set_value();
	return p.get_future();
}

std::ostream &operator<<(std::ostream &out, LogLevel level) {
	switch (level) {
	case LogLevel::Fatal:
		out << "fatal";
		break;
	case LogLevel::Error:
		out << "error";
		break;
	case LogLevel::Warning:
		out << "warning";
		break;
	case LogLevel::Info:
		out << "info";
		break;
	case LogLevel::Debug:
		out << "debug";
		break;
	case LogLevel::Verbose:
		out << "verbose";
		break;
	default:
		out << "none";
		break;
	}
	return out;
}

} // namespace rtc
