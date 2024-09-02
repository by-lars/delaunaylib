///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_POINTERS_H
#define GSL_POINTERS_H

#include <type_traits>

namespace gsl {
    //
    // owner
    //
    // `gsl::owner<T>` is designed as a safety mechanism for code that must deal directly with raw pointers that own
    // memory. Ideally such code should be restricted to the implementation of low-level abstractions. `gsl::owner` can
    // also be used as a stepping point in converting legacy code to use more modern RAII constructs, such as smart
    // pointers.
    //
    // T must be a pointer type
    // - disallow construction from any type other than pointer type
    //
    template<class T, class = std::enable_if_t<std::is_pointer_v<T>>>
    using owner = T;

}// namespace gsl
#endif// GSL_POINTERS_H
