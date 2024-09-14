#ifndef CIE_MESHKERNEL_ABS_MARCHING_PRIMITIVES_IMPL_HPP
#define CIE_MESHKERNEL_ABS_MARCHING_PRIMITIVES_IMPL_HPP

// --- Geo Includes ---
#include "packages/marchingprimitives/inc/AbsMarchingPrimitives.hpp"

// --- Utility Includes ---
#include "packages/maths/inc/bit.hpp"
#include "packages/concurrency/inc/ParallelFor.hpp"
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/checks.hpp"


namespace cie::geo {


template <concepts::Object TargetType>
AbsMarchingPrimitives<TargetType>::AbsMarchingPrimitives(target_ptr p_target,
                                                         const edge_table& r_edgeTable,
                                                         const connectivity_table& r_connectivityTable,
                                                         output_functor outputFunctor) :
    _p_target(p_target),
    _p_edgeTable(&r_edgeTable),
    _p_connectivityTable(&r_connectivityTable),
    _outputFunctor(outputFunctor)
{
}


template <concepts::Object TargetType>
template <class ...TStored>
void
AbsMarchingPrimitives<TargetType>::execute(Ref<mp::ThreadPool<mp::ThreadStorage<TStored...>>> r_pool)
{
    return executeImpl(std::optional<std::reference_wrapper<mp::ThreadPool<mp::ThreadStorage<TStored...>>>>(r_pool));
}


template <concepts::Object TargetType>
template <class ...TStored>
void
AbsMarchingPrimitives<TargetType>::execute()
{
    return executeImpl();
}


template <concepts::Object TargetType>
template <class ...TStored>
void
AbsMarchingPrimitives<TargetType>::executeImpl(std::optional<std::reference_wrapper<mp::ThreadPool<mp::ThreadStorage<TStored...>>>>  r_pool)
{
    CIE_BEGIN_EXCEPTION_TRACING

    this->checkInitialized();
    const Size numberOfPrimitivesToProcess = this->numberOfRemainingPrimitives();
    auto job = [this](Size primitiveIndex) -> void {
        Size configurationIndex = 0;
        const Size numberOfVerticesPerPrimitive = this->primitiveVertexCount();

        // Evaluate target and build configuration
        for (Size vertexIndex=0; vertexIndex<numberOfVerticesPerPrimitive; ++vertexIndex)
            if (!this->_p_target->at(this->getVertex(primitiveIndex,vertexIndex)))
                configurationIndex = utils::flipBit(configurationIndex, vertexIndex);

        // Find surface primitive constructor map
        const auto& r_edgeSets = this->_p_connectivityTable->at(configurationIndex);

        // Get edge indices for each output vertex
        typename AbsMarchingPrimitives<TargetType>::output_arguments outputArguments;

        for (const auto& r_edgeSet : r_edgeSets)
        {
            for (Size vertexIndex=0; vertexIndex<r_edgeSet.size(); ++vertexIndex)
                outputArguments[vertexIndex] = this->_p_edgeTable->at(r_edgeSet[vertexIndex]);

            this->_outputFunctor(primitiveIndex, outputArguments);
        }
    };

    if (r_pool) {
        mp::ParallelFor<> loop(r_pool.value().get());
        loop(numberOfPrimitivesToProcess, job);
    } else {
        for (Size primitiveIndex=0; primitiveIndex<numberOfPrimitivesToProcess; ++primitiveIndex) {
            job(primitiveIndex);
        }
    }

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TargetType>
inline void
AbsMarchingPrimitives<TargetType>::setOutputFunctor(typename AbsMarchingPrimitives<TargetType>::output_functor outputFunctor)
{
    this->_outputFunctor = outputFunctor;
}


template <concepts::Object TargetType>
void
AbsMarchingPrimitives<TargetType>::checkInitialized() const
{
    CIE_CHECK(_p_target, "Unset target function")
    CIE_CHECK(_p_edgeTable, "Unset edge table")
    CIE_CHECK(_p_connectivityTable, "Unset connectivity table")
    CIE_CHECK(_outputFunctor, "Unset output functor")
}


} // namespace cie::geo


#endif