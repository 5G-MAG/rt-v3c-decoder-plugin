/*
* Copyright (c) 2024 InterDigital R&D France
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include <array>
#include <cmath>
#include <execution>
#include <functional>
#include <iloj/misc/logger.h>
#include <numeric>
#include <vector>

struct cusparseDnVecDescr;
struct cusparseSpVecDescr;
struct cusparseSpMatDescr;

namespace iloj::math::sparse
{
////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief Element list
template<typename INDEX, typename VALUE>
class ElementList
{
public:
    enum class OrderType
    {
        None,
        CSR,
        CSC
    };

public:
    using index_type = INDEX;
    using value_type = VALUE;

private:
    std::vector<index_type> m_rowList{};
    std::vector<index_type> m_columnList{};
    std::vector<value_type> m_valueList{};
    OrderType m_orderType = OrderType::None;

public:
    //! \brief Default constructor
    ElementList() = default;
    //! \brief User-define constructor
    ElementList(std::vector<index_type> rowList,
                std::vector<index_type> columnList,
                std::vector<value_type> valueList,
                OrderType orderType = OrderType::None)
        : m_rowList{std::move(rowList)},
          m_columnList{std::move(columnList)},
          m_valueList{std::move(valueList)},
          m_orderType{orderType}
    {
    }
    //! \brief Default destructor
    ~ElementList() = default;
    //! \brief Copy constructor
    ElementList(const ElementList &) = default;
    //! \brief Move constructor
    ElementList(ElementList &&) noexcept = default;
    //! \brief Copy assignment
    auto operator=(const ElementList &) -> ElementList & = default;
    //! \brief Move assignment
    auto operator=(ElementList &&) noexcept -> ElementList & = default;
    //! \brief Returns list size
    [[nodiscard]] auto size() const -> std::size_t { return m_valueList.size(); }
    //! \brief Returns the row list
    auto rowList() -> std::vector<index_type> & { return m_rowList; }
    [[nodiscard]] auto rowList() const -> const std::vector<index_type> & { return m_rowList; }
    //! \brief Returns the column list
    auto columnList() -> std::vector<index_type> & { return m_columnList; }
    [[nodiscard]] auto columnList() const -> const std::vector<index_type> & { return m_columnList; }
    //! \brief Returns the value list
    auto valueList() -> std::vector<value_type> & { return m_valueList; }
    [[nodiscard]] auto valueList() const -> const std::vector<value_type> & { return m_valueList; }
    // Set / get current element list sorting type as specified by the end-user.
    auto setOrderType(OrderType orderType) { m_orderType = orderType; }
    [[nodiscard]] auto getOrderType() const -> OrderType { return m_orderType; }
    //! \brief Adjust list capacity
    void reserve(std::size_t sz)
    {
        m_rowList.reserve(sz);
        m_columnList.reserve(sz);
        m_valueList.reserve(sz);
    }
    //! \brief Append element
    void append(index_type i, index_type j, value_type value)
    {
        m_rowList.emplace_back(i);
        m_columnList.emplace_back(j);
        m_valueList.emplace_back(value);
    }
    //! \brief Resize
    void resize(std::size_t sz)
    {
        m_rowList.resize(sz);
        m_columnList.resize(sz);
        m_valueList.resize(sz);
    }
    //! \brief Set element id
    void set(std::size_t id, index_type i, index_type j, value_type value)
    {
        m_rowList[id] = i;
        m_columnList[id] = j;
        m_valueList[id] = value;
    }
    //! \brief Clear list
    void clear()
    {
        m_rowList.clear();
        m_columnList.clear();
        m_valueList.clear();
    }
    //! \brief Returns permutation list corresponding to order type
    [[nodiscard]] auto getPermutationList(OrderType orderType) const -> std::vector<index_type>
    {
        std::vector<index_type> permutationList(m_valueList.size());

        std::iota(permutationList.begin(), permutationList.end(), index_type{0});

        if (m_orderType != orderType)
        {
            const auto &firstList = (orderType == OrderType::CSC) ? m_columnList : m_rowList;
            // const auto &secondList = (orderType == OrderType::CSC) ? m_rowList : m_columnList;

            std::sort(
                std::execution::par_unseq, permutationList.begin(), permutationList.end(), [&](auto id1, auto id2) {
                    // if (firstList[id1] < firstList[id2])
                    // {
                    //     return true;
                    // }
                    // else if (firstList[id1] == firstList[id2])
                    // {
                    //     return (secondList[id1] < secondList[id2]);
                    // }

                    // return false;

                    return (firstList[id1] < firstList[id2]);
                });
        }

        return permutationList;
    }
};

//! \brief Returns permutation list corresponding to order type
template<typename INDEX, typename VALUE>
auto getMaxColumnSquaredNorm(const ElementList<INDEX, VALUE> &list, INDEX nbCols) -> VALUE
{
    std::vector<VALUE> columnNormList(nbCols, VALUE{0});
    const auto &columnList = list.columnList();
    const auto &valList = list.valueList();

    for (auto id = 0ULL; id < valList.size(); id++)
    {
        auto colId = columnList[id];
        auto val = valList[id];

        columnNormList[colId] += (val * val);
    }

    return *std::max_element(columnNormList.begin(), columnNormList.end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief CUDA-related Context
namespace impl
{
template<typename T>
auto getDataType() -> int;

template<typename T>
auto getIndexType() -> int;

} // namespace impl

namespace impl
{
class Buffer
{
private:
    std::size_t m_count{};
    std::size_t m_size{};
    void *m_ptr = nullptr;

public:
    //! \brief Default constructor
    Buffer() = default;
    //! \brief Default destructor
    virtual ~Buffer();
    //! \brief User-provide constuctor
    //! \param[in] count Number of elements
    //! \param[in] size Memory size in bytes
    //! \param[in] clear Clear data to 0 if true
    Buffer(std::size_t count, std::size_t size, bool clear);
    //! \brief User-provide constuctor
    //! \param[in] count Number of elements
    //! \param[in] size Memory size in bytes
    //! \param[in] ptr Pointer to data
    Buffer(std::size_t count, std::size_t size, const void *ptr);
    //! \brief Copy constructor
    Buffer(const Buffer & /*other*/);
    //! \brief Move constructor
    Buffer(Buffer &&other) noexcept;
    //! \brief Copy assignment
    auto operator=(const Buffer & /*other*/) -> Buffer &;
    //! \brief Move assignment
    auto operator=(Buffer &&other) noexcept -> Buffer &;
    //! \brief Returns memory size
    [[nodiscard]] auto getMemorySize() const -> std::size_t { return m_size; }
    //! \brief Returns number of elements
    [[nodiscard]] auto count() const -> std::size_t { return m_count; }
    //! \brief Returns pointer to device data.
    auto getDataPointer() -> void * { return m_ptr; }
    [[nodiscard]] auto getDataPointer() const -> const void * { return m_ptr; }
    template<typename T>
    auto getDataPointerAs() -> T *
    {
        return reinterpret_cast<T *>(m_ptr);
    }
    template<typename T>
    [[nodiscard]] [[nodiscard]] auto getDataPointerAs() const -> const T *
    {
        return reinterpret_cast<const T *>(m_ptr);
    }
    //! \brief Read / write data.
    void writeData(const void *ptr);
    void readData(void *ptr) const;
    //! \brief Read / write data for specific place / length
    void writeData(std::size_t offset, std::size_t size, const void *ptr) const;
    template<typename T>
    void writeData(std::size_t id, const T &value) const
    {
        writeData(id * sizeof(T), sizeof(T), reinterpret_cast<const void *>(&value));
    }
    void readData(std::size_t offset, std::size_t size, void *ptr) const;
    template<typename T>
    [[nodiscard]] auto readData(std::size_t id) const -> T
    {
        T result{};
        readData(id * sizeof(T), sizeof(T), reinterpret_cast<void *>(&result));
        return result;
    }
    //! \brief Clear values to 0.
    void clear();
};

} // namespace impl

template<typename T>
class Buffer: public impl::Buffer
{
public:
    using value_type = T;

public:
    //! \brief Default constructor
    Buffer() = default;
    //! \brief Default destructor
    ~Buffer() override = default;
    //! \brief User-provide constuctor
    //! \param[in] count Number of elements
    //! \param[in] clear Clear data to 0 if true
    Buffer(std::size_t count, bool clear = false): impl::Buffer{count, count * sizeof(T), clear} {}
    //! \brief User-provide constuctor
    //! \param[in] count Number of elements
    //! \param[in] ptr Pointer to data
    Buffer(std::size_t count, const T *ptr): impl::Buffer{count, count * sizeof(T), reinterpret_cast<const void *>(ptr)}
    {
    }
    //! \brief Copy constructor
    Buffer(const Buffer &) = default;
    //! \brief Move constructor
    Buffer(Buffer &&other) noexcept = default;
    //! \brief Copy assignment
    auto operator=(const Buffer &) -> Buffer & = default;
    //! \brief Move assignment
    auto operator=(Buffer &&other) noexcept -> Buffer & = default;
    //! \brief Returns pointer to device data.
    auto data() -> T * { return getDataPointerAs<T>(); }
    [[nodiscard]] auto data() const -> const T * { return getDataPointerAs<T>(); }
    //! \brief Returns idth value.
    auto operator[](std::size_t id) const -> T { return readData<T>(id); }
    //! \brief Write data to buffer.
    void write(const T *ptr) { writeData(reinterpret_cast<const void *>(ptr)); }
    template<typename CONTAINER>
    void operator<<(const CONTAINER &container)
    {
        resize(container.size() * sizeof(typename CONTAINER::value_type) / sizeof(T));
        writeData(reinterpret_cast<const void *>(container.data()));
    }
    //! \brief Read data from buffer.
    void read(T *ptr) const { readData(reinterpret_cast<void *>(ptr)); }
    template<typename CONTAINER>
    void operator>>(CONTAINER &container) const
    {
        container.resize(getMemorySize() / sizeof(typename CONTAINER::value_type));
        readData(reinterpret_cast<void *>(container.data()));
    }
    //! \brief Resize and optionnally clear.
    void resize(std::size_t count, bool clear = false)
    {
        if (this->count() != count)
        {
            *this = Buffer{count, clear};
        }
        else if (clear)
        {
            this->clear();
        }
    }
    //! \brief Returns a buffer filled with 0 to count - 1
    static auto IdentityPermutation(std::size_t count) -> Buffer<T>;
};

//! \brief Apply permutation to buffer
template<typename T>
void permute(const Buffer<T> &v, const Buffer<int> &p, Buffer<T> &output);

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace impl
{
class DenseVector
{
private:
    cusparseDnVecDescr *m_desc = nullptr;

public:
    //! \brief Default constructor
    DenseVector() = default;
    //! \brief User-provide constuctor
    //! \param[in] size Number of elements
    //! \param[in] values Pointer to data
    //! \param[in] dataType Data type
    DenseVector(std::size_t size, void *values, int dataType);
    //! \brief Default destructor
    virtual ~DenseVector();
    //! \brief Copy constructor
    DenseVector(const DenseVector &) = delete;
    //! \brief Move constructor
    DenseVector(DenseVector &&other) noexcept;
    //! \brief Copy assignment
    auto operator=(const DenseVector &) -> DenseVector & = delete;
    //! \brief Move assignment
    auto operator=(DenseVector &&other) noexcept -> DenseVector &;
    //! \brief Returns pointer to device data.
    auto getDescriptor() -> cusparseDnVecDescr * { return m_desc; }
    [[nodiscard]] auto getDescriptor() const -> cusparseDnVecDescr * { return m_desc; }
};

} // namespace impl

template<typename T>
class DenseVector: public impl::DenseVector
{
public:
    using value_type = T;

private:
    Buffer<T> m_buffer;

public:
    //! \brief Default constructor
    DenseVector() = default;
    //! \brief Default destructor
    ~DenseVector() override = default;
    //! \brief User-provide constuctor
    //! \param[in] buffer Data buffer
    DenseVector(Buffer<T> buffer)
        : impl::DenseVector{buffer.count(), buffer.getDataPointer(), impl::getDataType<T>()},
          m_buffer{std::move(buffer)}
    {
    }
    //! \brief User-provide constuctor
    //! \param[in] size Number of elements
    //! \param[in] clear Clear data to 0 if true
    //! \param[in] dataType Data type
    DenseVector(std::size_t size, bool clear = false): DenseVector{Buffer<T>{size, clear}} {}
    //! \brief Copy constructor
    DenseVector(const DenseVector &other): DenseVector{other.buffer()} {}
    //! \brief Move constructor
    DenseVector(DenseVector &&other) noexcept = default;
    //! \brief Copy assignment
    auto operator=(const DenseVector &other) -> DenseVector &
    {
        *this = DenseVector{other.buffer()};
        return *this;
    }
    //! \brief Move assignment
    auto operator=(DenseVector &&other) noexcept -> DenseVector & = default;
    //! \brief Returns idth value.
    auto operator[](std::size_t id) const -> T { return m_buffer[id]; }
    //! \brief Returns vector size
    [[nodiscard]] auto size() const -> std::size_t { return m_buffer.count(); }
    //! \brief Returns pointer to device data.
    auto buffer() -> Buffer<T> & { return m_buffer; }
    [[nodiscard]] auto buffer() const -> const Buffer<T> & { return m_buffer; }
    //! \brief Write data to buffer.
    template<typename CONTAINER>
    void operator<<(const CONTAINER &container)
    {
        resize(container.size() * sizeof(typename CONTAINER::value_type) / sizeof(T));
        m_buffer << container;
    }
    //! \brief Read data from buffer.
    template<typename CONTAINER>
    void operator>>(CONTAINER &container) const
    {
        container.resize(m_buffer.getMemorySize() / sizeof(typename CONTAINER::value_type));
        m_buffer >> container;
    }
    //! \brief Clear values to 0.
    void clear() { m_buffer.clear(); }
    //! \brief Resize (without preserving content)
    //! \param[in] size Number of elements
    //! \param[in] clear Clear data to 0 if true
    void resize(std::size_t size, bool clear = false)
    {
        if (this->size() != size)
        {
            *this = DenseVector{size, clear};
        }
        else if (clear)
        {
            this->clear();
        }
    }
};

//! \brief y = x + alpha
template<typename T>
void add(float alpha, const DenseVector<T> &in, DenseVector<T> &out);

//! \brief y = x - alpha
template<typename T>
void sub(float alpha, const DenseVector<T> &in, DenseVector<T> &out);

//! \brief y = x * alpha
template<typename T>
void scale(float alpha, const DenseVector<T> &in, DenseVector<T> &out);

//! \brief y = x * x
template<typename T>
void square(const DenseVector<T> &in, DenseVector<T> &out);

//! \brief z = x + y
template<typename T>
void add(const DenseVector<T> &first, const DenseVector<T> &second, DenseVector<T> &out);

//! \brief z = x - y
template<typename T>
void sub(const DenseVector<T> &first, const DenseVector<T> &second, DenseVector<T> &out);

//! \brief z = alpha * x + beta * y
template<typename T>
void linear(T alpha, const DenseVector<T> &first, T beta, const DenseVector<T> &second, DenseVector<T> &out);

//! \brief z = x * y
template<typename T>
void mult(const DenseVector<T> &first, const DenseVector<T> &second, DenseVector<T> &out);

//! \brief Returns min(v)
template<typename T>
auto min(const DenseVector<T> &v) -> T;

//! \brief Returns max(v)
template<typename T>
auto max(const DenseVector<T> &v) -> T;

//! \brief Returns sum(v)
template<typename T>
auto sum(const DenseVector<T> &v) -> T;

//! \brief Returns norm_inf(v)
template<typename T>
auto norm_inf(const DenseVector<T> &v) -> T;

//! \brief Returns norm2(v)
template<typename T>
auto norm2(const DenseVector<T> &v) -> T;

//! \brief Returns ||v||
template<typename T>
auto norm(const DenseVector<T> &v) -> T
{
    return std::sqrt(norm2(v));
}

//! \brief Returns dot(v1, v2)
template<typename T>
auto dot(const DenseVector<T> &v1, const DenseVector<T> &v2) -> T;

//! \brief Clamp h so that minValue <= x + h <= maxValue
template<typename T>
void stepbox(const DenseVector<T> &x, T minValue, T maxValue, DenseVector<T> &h);

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace impl
{
class SparseMatrix
{
private:
    std::array<std::size_t, 2> m_size{};
    std::size_t m_nnz{};
    cusparseSpMatDescr *m_desc = nullptr;

public:
    //! \brief Default constructor
    SparseMatrix() = default;
    //! \brief User-provide constuctor
    //! \param[in] m Number of rows
    //! \param[in] n Number of columns
    //! \param[in] nnz Number of non zero elements
    //! \param[in] indices Pointer to offset data
    //! \param[in] indices Pointer to index data
    //! \param[in] values Pointer to value data
    //! \param[in] offsetType Offset type
    //! \param[in] indexType Index type
    //! \param[in] valueType Value type
    SparseMatrix(std::size_t m,
                 std::size_t n,
                 std::size_t nnz,
                 void *offsetList,
                 void *indexList,
                 void *valueList,
                 int offsetType,
                 int indexType,
                 int valueType);
    //! \brief Default destructor
    virtual ~SparseMatrix();
    //! \brief Copy constructor
    SparseMatrix(const SparseMatrix &) = delete;
    //! \brief Move constructor
    SparseMatrix(SparseMatrix &&other) noexcept;
    //! \brief Copy assignment
    auto operator=(const SparseMatrix &) -> SparseMatrix & = delete;
    //! \brief Move assignment
    auto operator=(SparseMatrix &&other) noexcept -> SparseMatrix &;
    //! \brief Returns number of rows
    [[nodiscard]] auto m() const -> std::size_t { return m_size[0]; }
    //! \brief Returns number of columns
    [[nodiscard]] auto n() const -> std::size_t { return m_size[1]; }
    //! \brief Returns number of nnz
    [[nodiscard]] auto nnz() const -> std::size_t { return m_nnz; }
    //! \brief Returns pointer to device data.
    auto getDescriptor() -> cusparseSpMatDescr * { return m_desc; }
    [[nodiscard]] auto getDescriptor() const -> cusparseSpMatDescr * { return m_desc; }
};

} // namespace impl

template<typename OFFSET, typename INDEX, typename VALUE>
class SparseMatrix: public impl::SparseMatrix
{
public:
    using offset_type = OFFSET;
    using index_type = INDEX;
    using value_type = VALUE;

private:
    Buffer<OFFSET> m_offsetBuffer;
    Buffer<INDEX> m_indexBuffer;
    Buffer<VALUE> m_valueBuffer;

public:
    //! \brief Default constructor
    SparseMatrix() = default;
    //! \brief Default destructor
    ~SparseMatrix() override = default;
    //! \brief User-provide constuctor
    //! \param[in] m Number of rows
    //! \param[in] n Number of columns
    //! \param[in] offsetBuffer Offset buffer
    //! \param[in] indexBuffer Index buffer
    //! \param[in] valueBuffer Value buffer
    SparseMatrix(std::size_t m,
                 std::size_t n,
                 Buffer<OFFSET> offsetBuffer,
                 Buffer<INDEX> indexBuffer,
                 Buffer<VALUE> valueBuffer)
        : impl::SparseMatrix{m,
                             n,
                             valueBuffer.count(),
                             offsetBuffer.getDataPointer(),
                             indexBuffer.getDataPointer(),
                             valueBuffer.getDataPointer(),
                             impl::getIndexType<OFFSET>(),
                             impl::getIndexType<INDEX>(),
                             impl::getDataType<VALUE>()},
          m_offsetBuffer{std::move(offsetBuffer)},
          m_indexBuffer{std::move(indexBuffer)},
          m_valueBuffer{std::move(valueBuffer)}
    {
    }
    //! \brief Copy constructor
    SparseMatrix(const SparseMatrix &) = delete;
    //! \brief Move constructor
    SparseMatrix(SparseMatrix &&other) noexcept = default;
    //! \brief Copy assignment
    auto operator=(const SparseMatrix &) -> SparseMatrix & = delete;
    //! \brief Move assignment
    auto operator=(SparseMatrix &&other) noexcept -> SparseMatrix & = default;
    //! \brief Returns pointer to device index data.
    auto getOffsetBuffer() -> Buffer<OFFSET> & { return m_offsetBuffer; }
    [[nodiscard]] auto getOffsetBuffer() const -> const Buffer<OFFSET> & { return m_offsetBuffer; }
    //! \brief Returns pointer to device index data.
    auto getIndexBuffer() -> Buffer<INDEX> & { return m_indexBuffer; }
    [[nodiscard]] auto getIndexBuffer() const -> const Buffer<INDEX> & { return m_indexBuffer; }
    //! \brief Returns pointer to device value data.
    auto getValueBuffer() -> Buffer<VALUE> & { return m_valueBuffer; }
    [[nodiscard]] auto getValueBuffer() const -> const Buffer<VALUE> & { return m_valueBuffer; }
    //! \brief Returns a sparse matrix from an element list
    //! \param[in] m Number of rows
    //! \param[in] n Number of columns
    //! \param[in] list Non zero element list
    //! \param[in] transpose Transpose if true
    static auto
    fromElementList(std::size_t m, std::size_t n, const ElementList<INDEX, VALUE> &list, bool transpose = false)
        -> SparseMatrix<OFFSET, INDEX, VALUE>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace impl
{
void matprod(

    const void *alpha,
    const SparseMatrix &A,
    char mA,
    const DenseVector &x,
    const void *beta,
    DenseVector &y,
    int computeType);
}

//! \brief y = alpha * A * x + alpha * y if mA == 'N' else y = alpha * A' * x + alpha * y.
template<typename OFFSET, typename INDEX, typename VALUE>
auto matprod(VALUE alpha,
             const SparseMatrix<OFFSET, INDEX, VALUE> &A,
             char mA,
             const DenseVector<VALUE> &x,
             VALUE beta,
             DenseVector<VALUE> &y)
{
    if ((mA == 'N') && (y.size() != A.m()))
    {
        y.resize(A.m(), true);
    }
    else if ((mA != 'N') && (y.size() != A.n()))
    {
        y.resize(A.n(), true);
    }

    return impl::matprod(&alpha, A, mA, x, &beta, y, impl::getDataType<VALUE>());
}

//! Brief LSQR context class
template<typename MAT>
class LSQR
{
public:
    using value_type = typename MAT::value_type;
    using Vector = DenseVector<value_type>;
    using Matrix = MAT;

public:
    //! \brief Parameter class to provide as input of a LSQR context
    //! atol, btol  are stopping tolerances.  If both are 1.0e-9 (say),
    //!             the final residual norm should be accurate to about 9 digits.
    //!             (The final x will usually have fewer correct digits,
    //!             depending on cond(A) and the size of damp.)
    //! conlim      is also a stopping tolerance.  lsqr terminates if an estimate
    //!             of cond(A) exceeds conlim. For compatible systems Ax = b,
    //!             conlim could be as large as 1.0e+12 (say).  For least-squares
    //!             problems, conlim should be less than 1.0e+8.
    //!             Maximum precision can be obtained by setting
    //!             atol = btol = conlim = zero, but the number of iterations
    //!             may then be excessive.
    //! itnlim      is an explicit limit on iterations (for safety).
    class Parameter
    {
    public:
        value_type m_atol{};
        value_type m_btol{};
        value_type m_conlim{};
        std::size_t m_itnlim{};

    public:
        auto getAToleranceThreshold() const -> value_type { return m_atol; }
        auto getBToleranceThreshold() const -> value_type { return m_btol; }
        auto getConditionNumberLimit() const -> value_type { return m_conlim; }
        [[nodiscard]] auto getMaxNumberOfIteration() const -> std::size_t { return m_itnlim; }
    };

    //! \brief Status class returned by a LSQR solver
    //! itn       Number of iterations at completion
    //! rnorm    ||Ax - b||^2 + damp^2*||x||^2 at completion
    //! code      Return code at completion
    class Status
    {
    public:
        std::size_t m_itn{};
        value_type m_residue;
        int m_code{-1};

    public:
        [[nodiscard]] auto getNumberOfIteration() const -> std::size_t { return m_itn; }
        auto getResidueNorm() const -> const std::vector<value_type> & { return m_residue; }
        [[nodiscard]] auto getReturnCode() const -> int { return m_code; }
        [[nodiscard]] auto getMessage() const -> const std::string &;
    };

private:
    Parameter m_parameter{};

public:
    //! \brief Default constructor
    LSQR() = default;
    //! \brief Default destructor
    ~LSQR() = default;
    //! Brief Construct a LSQR context with the provided parameters
    LSQR(const Parameter &parameter): m_parameter{parameter} {}
    //! \brief Copy constructor
    LSQR(const LSQR &) = default;
    //! \brief Move constructor
    LSQR(LSQR &&) noexcept = default;
    //! \brief Copy assignment
    auto operator=(const LSQR &) -> LSQR & = default;
    //! \brief Move assignment
    auto operator=(LSQR &&) noexcept -> LSQR & = default;
    //! \brief Solves  Ax = b  or  min ||b - Ax||_2  if damp = 0,
    //! or   min || (b)  -  (  A   )x ||   otherwise.
    //!          || (0)     (damp I)  ||2 vector.
    //! \param[in] A Left-hand side sparse matrix.
    //! \param[in] b Right-hand side vector.
    //! \param[in] damp Damp factor.
    //! \param[inout] x Solution when found.
    //! \return Output status.
    auto solve(const Matrix &A, const Vector &b, value_type damp, Vector &x) const -> Status;
};

template<typename MAT>
struct LM
{
    using index_type = typename MAT::index_type;
    using value_type = typename MAT::value_type;

    using List = ElementList<index_type, value_type>;
    using Vector = DenseVector<value_type>;
    using Matrix = MAT;

    using CostFunction = std::function<void(const Vector &, Vector &, List &)>;

    static auto getImprovement(value_type first, value_type last) -> value_type { return (first - last) / first; }

    using LoggingCallback = std::function<void(std::size_t, value_type, const Vector &, const Vector &)>;

    static void
    defaultLoggingCallback(std::size_t iter, value_type improvement, const Vector & /* x */, const Vector & /* f */)
    {
        LOG_INFO("Iter #", iter, ": ", improvement * 100, '%');
    }

    //! \brief Parameter class to provide as input of an optimizer
    //! logging                 enable logging during optimization process
    //! maxNumberIteration      is an explicit limit on iterations (for safety).
    //! objectiveTolerance      causes completion if ||f||int <= objectiveTolerance
    //! gradientTolerance       causes completion if ||g||inf <= gradientTolerance
    //! stepTolerance           causes completion if ||h|| <= stepTolerance (||x|| + stepTolerance)
    //! improvementTolerance    causes completion if (||f|| - ||f||new) <= improvementTolerance * ||f||
    //! tau         as a rule of thumb, one should use a small value, eg 1e-6 if x0 is believed
    //!             to be a good approximation to x* otherwise, use 1e-3 or even 1

    class Parameter
    {
    public:
        std::size_t m_maxNumberIteration{};
        value_type m_objectiveTolerance{};
        value_type m_gradientTolerance{};
        value_type m_stepTolerance{};
        value_type m_improvementTolerance{};
        value_type m_tau{};

    public:
        [[nodiscard]] auto getMaxNumberOfIteration() const -> std::size_t { return m_maxNumberIteration; }
        auto getObjectiveTolerance() const -> value_type { return m_objectiveTolerance; }
        auto getGradientTolerance() const -> value_type { return m_gradientTolerance; }
        auto getStepTolerance() const -> value_type { return m_stepTolerance; }
        auto getImprovementTolerance() const -> value_type { return m_improvementTolerance; }
        auto getTau() const -> value_type { return m_tau; }
    };

    //! \brief Status class returned by an optimizer
    //! itn           Number of iterations at completion
    //! improvement   Improvement in percentage of the initial cost
    //! fnorm         ||f||inf at completion
    //! gnorm         ||grad(f)||inf at completion
    //! code          Return code at completion
    //! message       Human readable completion reason
    class Status
    {
    public:
        enum CodeId
        {
            MaxIter = 0,
            ObjectiveTol,
            GradientTol,
            StepTol,
            ImprovementTol
        };

    public:
        std::size_t m_itn{};
        value_type m_fnorm{};
        value_type m_gnorm{};
        value_type m_improvement{};
        int m_code{-1};

    public:
        [[nodiscard]] auto getNumberOfIteration() const -> std::size_t { return m_itn; }
        auto getObjectiveNorm() const -> value_type { return m_fnorm; }
        auto getGradientNorm() const -> value_type { return m_gnorm; }
        auto getImprovement() const -> value_type { return m_improvement; }
        [[nodiscard]] auto getCode() const -> int { return m_code; }
        [[nodiscard]] auto getMessage() const -> const std::string &;
        void toStream(std::ostream &os) const;
    };

    using BoxConstraint = std::pair<value_type, value_type>;

    //! \brief LM (Levenberg-Marquardt) optimizer context class
    class Optimizer
    {
    private:
        LSQR<MAT> m_lsqrContext{};
        Parameter m_lmParameter{};

    public:
        //! \brief Default constructor
        Optimizer() = default;
        //! \brief Default destructor
        ~Optimizer() = default;
        //! Brief Construct a LM context with the provided parameters
        Optimizer(const typename LSQR<MAT>::Parameter &lsqrParams, const Parameter &lmParams)
            : m_lsqrContext{lsqrParams}, m_lmParameter{lmParams}
        {
        }
        //! \brief Copy constructor
        Optimizer(const Optimizer &) = default;
        //! \brief Move constructor
        Optimizer(Optimizer &&) noexcept = default;
        //! \brief Copy assignment
        auto operator=(const Optimizer &) -> Optimizer & = default;
        //! \brief Move assignment
        auto operator=(Optimizer &&) noexcept -> Optimizer & = default;
        //! \brief Minimizes the provided cost function
        //! \param[in] costFunction Function to minimize with sparse Jacobian
        //! \param[inout] x Initial as well as final solution
        //! \param[in] xLow Optional lower bound constraint
        //! \param[in] xHigh Optional upper bound constraint
        //! \return Output status
        auto minimize(const CostFunction &costFunction,
                      Vector &x,
                      const BoxConstraint &boxConstraint = {},
                      const LoggingCallback &onLogging = defaultLoggingCallback) const -> Status;

    private:
        void applyBoxConstraint(const BoxConstraint &boxConstraint, const Vector &x, Vector &h) const;
    };
};

} // namespace iloj::math::sparse

#include "sparse.hpp"