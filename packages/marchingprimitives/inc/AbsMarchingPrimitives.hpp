#ifndef CIE_MESHKERNEL_ABS_MARCHING_PRIMITIVES_HPP
#define CIE_MESHKERNEL_ABS_MARCHING_PRIMITIVES_HPP

// --- Utility Includes ---
#include "packages/concurrency/inc/ThreadPool.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Geo Includes ---
#include "packages/primitives/inc/Traits.hpp"

// --- STL Includes ---
#include <memory>
#include <utility>
#include <optional>


namespace cie::geo {


template <concepts::Object TTarget>
class AbsMarchingPrimitives : public GetTraitsT<TTarget>
{
public:
    using target_type           = TTarget;

    using target_ptr            = std::shared_ptr<target_type>;

    using edge_type             = std::pair<unsigned short,unsigned short>;

    using edge_table            = std::vector<edge_type>;

    using connectivity_table    = std::vector<std::vector<StaticArray<unsigned short,AbsMarchingPrimitives::Dimension>>>;

    /// Edge indices for every output vertex
    using output_arguments      = StaticArray<edge_type,AbsMarchingPrimitives<TTarget>::Dimension>;

    /// Primitive index and output_arguments
    using output_functor        = std::function<void(Size,const output_arguments&)>;

    using typename GetTraitsT<TTarget>::Point;

public:
    AbsMarchingPrimitives(target_ptr p_target,
                          const edge_table& r_edgeTable,
                          const connectivity_table& r_connectivityTable,
                          output_functor outputFunctor);

    virtual ~AbsMarchingPrimitives() {}

    /** @brief Collect boundary triangles and invoke the stored output function on them (parallel).
     *
     *  @tparam TStored: parameter pack of types stored in thread local storage.
     *  @param r_pool: thread pool to use in the parallel for loop.
     */
    template <class ...TStored>
    void execute(Ref<mp::ThreadPool<mp::ThreadStorage<TStored...>>> r_pool);

    /** @brief Collect boundary triangles and invoke the stored output function on them (serial).
     *  @tparam TStored: parameter pack of types stored in thread local storage.
     */
    template <class ...TStored>
    void execute();

    void setOutputFunctor(output_functor outputFunctor);

private:
    /// Make sure that everything is set up to perform the march
    void checkInitialized() const;

protected:
    AbsMarchingPrimitives() = delete;

    AbsMarchingPrimitives(const AbsMarchingPrimitives<TTarget>& r_rhs) = delete;

    AbsMarchingPrimitives<TTarget>& operator=(const AbsMarchingPrimitives<TTarget>& r_rhs) = delete;

    /// Vertex generator
    virtual Point getVertex(Size primitiveIndex,
                            Size vertexIndex) = 0;

    virtual void getVertices(Size primitiveIndex,
                             Ref<DynamicArray<Point>> r_vertexContainer) const = 0;

    /// Return the number of vertices that form a primitive.
    virtual Size primitiveVertexCount() const = 0;

    /// Return the number of remaining primitives to be scanned.
    virtual Size numberOfRemainingPrimitives() const = 0;

protected:
    /// Ptr to the target geometry
    target_ptr _p_target;

    /// Map edge index to vertex pairs
    Ptr<const edge_table> _p_edgeTable;

    /// Map active point configurations to surface primitives (3D: active point config -> triangles)
    Ptr<const connectivity_table> _p_connectivityTable;

    /// Function that gets called for every surface primitive
    output_functor _outputFunctor;

private:
    template <class ...TStored>
    void executeImpl(std::optional<std::reference_wrapper<mp::ThreadPool<mp::ThreadStorage<TStored...>>>> r_pool
                     = std::optional<std::reference_wrapper<mp::ThreadPool<mp::ThreadStorage<TStored...>>>>());
};



template <concepts::Object TTarget>
using MarchingPrimitivesPtr = std::shared_ptr<AbsMarchingPrimitives<TTarget>>;


} // namespace cie::geo

#include "packages/marchingprimitives/impl/AbsMarchingPrimitives_impl.hpp"

#endif