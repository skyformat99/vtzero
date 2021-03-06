#ifndef VTZERO_TYPES_HPP
#define VTZERO_TYPES_HPP

#include <protozero/types.hpp>

namespace vtzero {

    /**
     * Using data_view class from protozero. See the protozero documentation
     * on how to change this to use different implementations.
     * https://github.com/mapbox/protozero/blob/master/doc/advanced.md#protozero_use_view
     */
    using data_view = protozero::data_view;

    // based on https://github.com/mapbox/vector-tile-spec/blob/master/2.1/vector_tile.proto

    enum class GeomType {
        UNKNOWN    = 0,
        POINT      = 1,
        LINESTRING = 2,
        POLYGON    = 3
    };

    inline const char* geom_type_name(GeomType type) noexcept {
        static const char* names[] = {
            "unknown", "point", "linestring", "polygon"
        };
        return names[static_cast<int>(type)];
    }

    enum class value_type : protozero::pbf_tag_type {
        string_value = 1,
        float_value  = 2,
        double_value = 3,
        int_value    = 4,
        uint_value   = 5,
        sint_value   = 6,
        bool_value   = 7
    };

    inline const char* value_type_name(value_type type) noexcept {
        static const char* names[] = {
            "", "string", "float", "double", "int", "uint", "sint", "bool"
        };
        return names[static_cast<int>(type)];
    }

    namespace detail {

        enum class pbf_tile : protozero::pbf_tag_type {
            layers = 3
        };

        enum class pbf_layer : protozero::pbf_tag_type {
            name     =  1,
            features =  2,
            keys     =  3,
            values   =  4,
            extent   =  5,
            version  = 15
        };

        enum class pbf_feature : protozero::pbf_tag_type {
            id       = 1,
            tags     = 2,
            type     = 3,
            geometry = 4
        };

        using pbf_value = value_type;

    } // namespace detail

    struct string_value_type {
        data_view value;
    };

    struct float_value_type {
        float value;
    };

    struct double_value_type {
        double value;
    };

    struct int_value_type {
        int64_t value;
    };

    struct uint_value_type {
        uint64_t value;
    };

    struct sint_value_type {
        int64_t value;
    };

    struct bool_value_type {
        bool value;
    };

} // namespace vtzero

#endif // VTZERO_TYPES_HPP
