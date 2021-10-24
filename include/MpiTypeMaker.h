#include <mpi.h>
#include <type_traits>
#include <array>
#include <vector>
#include <tuple>

namespace MpiTypeMaker
{
    template <typename T>
    struct is_std_array : public std::false_type
    {
    };

    template <typename V, size_t n>
    struct is_std_array<std::array<V, n>> : public std::true_type
    {
    };

    template <typename T>
    struct GetInnerType
    {
        using type = T;
    };

    template <class T, size_t N>
    struct GetInnerType<T[N]>
    {
        using type = T;
    };

    template <class T>
    struct GetInnerType<T[]>
    {
        using type = T;
    };

    template <typename T, size_t n>
    struct GetInnerType<std::array<T, n>>
    {
        using type = T;
    };

    template <typename T, size_t size>
    size_t GetCArraySize(T (&)[size])
    {
        return size;
    }

    template <class T>
    auto GetMpiType(const T &x)
    {
        using U = GetInnerType<T>::type;

        static_assert(!std::is_pointer_v<U>, "T must NOT be a pointer.");

        if (std::is_same_v<U, int>)
            return MPI_INT;
        else if (std::is_same_v<U, long long int>)
            return MPI_LONG_LONG_INT;
        else if (std::is_same_v<U, float>)
            return MPI_FLOAT;
        else if (std::is_same_v<U, double>)
            return MPI_DOUBLE;
        else if (std::is_same_v<U, char>)
            return MPI_CHAR;
    }

    struct CustomTypeInfo
    {
        size_t size;
        std::vector<int> blocklengths;
        std::vector<MPI_Datatype> types;
        std::vector<MPI_Aint> displacements;

        auto resize(size_t n)
        {
            size = n;
            blocklengths.resize(n);
            types.resize(n);
            displacements.resize(n);
        }
    };

    template <class... Ts>
    auto GetStructInfo(const Ts &...all)
    {

        const int membersCount = sizeof...(Ts) - 1;
        CustomTypeInfo typeInfo{};
        typeInfo.resize(membersCount);

        processStructMember(typeInfo, 0, all...);

        return typeInfo;
    }

    // A recursive method to loop over all members
    template <class Tobj, class Tm, class... Ts>
    constexpr void processStructMember(CustomTypeInfo &typeInfo,
                                       int counter,
                                       const Tobj &obj, const Tm &member, const Ts &...rest)
    {

        if constexpr (is_std_array<Tm>::value)
            typeInfo.blocklengths[counter] = member.size();
        else if constexpr (std::is_array_v<std::remove_reference_t<Tm>>)
            typeInfo.blocklengths[counter] = GetCArraySize(member);
        else
            typeInfo.blocklengths[counter] = 1;

        typeInfo.displacements[counter] = MPI_Aint_diff(&member, &obj);

        typeInfo.types[counter] = GetMpiType(member);

        // Call itself if there are more members
        if constexpr (sizeof...(rest) > 0)
        {
            counter++;
            processStructMember(typeInfo, counter, obj, rest...);
        }
    }

    template <class T>
    auto GetTupleInfo(T tup)
    {

        const int membersCount = std::tuple_size<T>();
        CustomTypeInfo typeInfo{};
        typeInfo.resize(membersCount);

        processTupleMember<T, 0>(typeInfo, tup);

        return typeInfo;
    }
    template <class Ttup, int counter>
    auto processTupleMember(CustomTypeInfo &typeInfo, Ttup &obj)
    {
        auto &member = std::get<counter>(obj);
        using Tm = std::remove_reference_t<decltype(get<counter>(obj))>;

        if constexpr (is_std_array<Tm>::value)
            typeInfo.blocklengths[counter] = member.size();
        else if constexpr (std::is_array_v<std::remove_reference_t<Tm>>)
            typeInfo.blocklengths[counter] = GetCArraySize(member);
        else
            typeInfo.blocklengths[counter] = 1;

        typeInfo.displacements[counter] = MPI_Aint_diff(&member, &obj);

        typeInfo.types[counter] = GetMpiType(member);

        if constexpr (counter < std::tuple_size<Ttup>() - 1)
            processTupleMember<Ttup, counter + 1>(typeInfo, obj);
    }

    // This function is an API of this code.
    // It returns a custom struct MPI type.
    // The first argument is an object and
    // the rests are data members of that object.
    // The order of members is not important.
    template <class... Ts>
    auto CreateCustomMpiType(const Ts &...all)
    {

        auto typeInfo = GetStructInfo(all...);

        MPI_Datatype CustomType;

        MPI_Type_create_struct(typeInfo.size, typeInfo.blocklengths.data(), typeInfo.displacements.data(), typeInfo.types.data(), &CustomType);
        MPI_Type_commit(&CustomType);

        return CustomType;
    }

    // This function is an API of this code.
    // It returns a Tuple MPI type for the argument
    // which is a C++ tuple.
    template <class T>
    auto CreateTupleMpiType(T &tup)
    {

        auto typeInfo = GetTupleInfo(tup);

        MPI_Datatype CustomType;

        MPI_Type_create_struct(typeInfo.size, typeInfo.blocklengths.data(), typeInfo.displacements.data(), typeInfo.types.data(), &CustomType);
        MPI_Type_commit(&CustomType);

        return CustomType;
    }

}
