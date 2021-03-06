#ifndef VTZERO_VECTOR_TILE_HPP
#define VTZERO_VECTOR_TILE_HPP

#include "exception.hpp"
#include "layer.hpp"
#include "types.hpp"

#include <protozero/pbf_message.hpp>

#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

namespace vtzero {

    class layer_iterator {

        protozero::pbf_message<detail::pbf_tile> m_tile_reader;
        data_view m_data;

        void next() {
            try {
                if (m_tile_reader.next(detail::pbf_tile::layers,
                                    protozero::pbf_wire_type::length_delimited)) {
                    m_data = m_tile_reader.get_view();
                } else {
                    m_data = data_view{};
                }
            } catch (const protozero::exception&) {
                // convert protozero exceptions into vtzero exception
                throw protocol_buffers_exception{};
            }
        }

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type        = layer;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;

        /**
         * Construct special "end" iterator.
         */
        layer_iterator() = default;

        /**
         * Construct layer iterator from specified vector tile data.
         *
         * @throws format_exception if the tile data is ill-formed.
         */
        layer_iterator(const data_view& tile_data) :
            m_tile_reader(tile_data),
            m_data() {
            next();
        }

        layer operator*() const {
            assert(m_data.data() != nullptr);
            return layer{m_data};
        }

        layer operator->() const {
            assert(m_data.data() != nullptr);
            return layer{m_data};
        }

        /**
         * @throws format_exception if the tile data is ill-formed.
         */
        layer_iterator& operator++() {
            next();
            return *this;
        }

        /**
         * @throws format_exception if the tile data is ill-formed.
         */
        layer_iterator operator++(int) {
            const layer_iterator tmp{*this};
            ++(*this);
            return tmp;
        }

        bool operator==(const layer_iterator& other) const noexcept {
            return m_data == other.m_data;
        }

        bool operator!=(const layer_iterator& other) const noexcept {
            return !(*this == other);
        }

    }; // layer_iterator

    /**
     * A vector tile is basically nothing more than an ordered collection
     * of named layers. Use the subscript operator to access a layer by index
     * or name. Use begin()/end() to iterator over the layers.
     */
    class vector_tile {

        data_view m_data;

    public:

        explicit vector_tile(const data_view& data) noexcept :
            m_data(data) {
        }

        explicit vector_tile(const std::string& data) noexcept :
            m_data(data.data(), data.size()) {
        }

        /// Returns an iterator to the beginning.
        layer_iterator begin() const {
            return layer_iterator{m_data};
        }

        /// Returns an iterator to the end.
        layer_iterator end() const {
            return layer_iterator{};
        }

        /**
         * Returns the layer with the specified zero-based index.
         *
         * Complexity: Linear in the number of layers.
         *
         * @returns The specified layer or the invalid layer if index is
         *          larger than the number of layers.
         * @throws format_exception if the tile data is ill-formed.
         */
        layer operator[](std::size_t index) const {
            protozero::pbf_message<detail::pbf_tile> reader{m_data};

            try {
                while (reader.next(detail::pbf_tile::layers, protozero::pbf_wire_type::length_delimited)) {
                    if (index == 0) {
                        return layer{reader.get_view()};
                    }
                    reader.skip();
                    --index;
                }
            } catch (const protozero::exception&) {
                // convert protozero exceptions into vtzero exception
                throw protocol_buffers_exception{};
            }

            return layer{};
        }

        /**
         * Returns the layer with the specified name.
         *
         * Complexity: Linear in the number of layers.
         *
         * If there are several layers with the same name (which is against
         * the spec 4.1 "A Vector Tile MUST NOT contain two or more layers
         * whose name values are byte-for-byte identical.") it is unspecified
         * which will be returned.
         *
         * @returns The specified layer or the invalid layer if there is no
         *          layer with this name.
         * @throws format_exception if the tile data is ill-formed.
         */
        layer operator[](const data_view& name) const {
            protozero::pbf_message<detail::pbf_tile> reader{m_data};

            try {
                while (reader.next(detail::pbf_tile::layers, protozero::pbf_wire_type::length_delimited)) {
                    const auto layer_data = reader.get_view();
                    protozero::pbf_message<detail::pbf_layer> layer_reader{layer_data};
                    if (layer_reader.next(detail::pbf_layer::name, protozero::pbf_wire_type::length_delimited)) {
                        if (layer_reader.get_view() == name) {
                            return layer{layer_data};
                        }
                    } else {
                        // 4.1 "A layer MUST contain a name field."
                        throw format_exception{"missing name in layer (spec 4.1)"};
                    }
                }
            } catch (const protozero::exception&) {
                // convert protozero exceptions into vtzero exception
                throw protocol_buffers_exception{};
            }

            return layer{};
        }

        /**
         * Returns the layer with the specified name.
         *
         * Complexity: Linear in the number of layers.
         *
         * If there are several layers with the same name (which is against
         * the spec 4.1 "A Vector Tile MUST NOT contain two or more layers
         * whose name values are byte-for-byte identical.") it is unspecified
         * which will be returned.
         *
         * @returns The specified layer or the invalid layer if there is no
         *          layer with this name.
         * @throws format_exception if the tile data is ill-formed.
         */
        layer operator[](const std::string& name) const {
            const data_view dv{name.data(), name.size()};
            return (*this)[dv];
        }

        /**
         * Returns the layer with the specified name.
         *
         * Complexity: Linear in the number of layers.
         *
         * If there are several layers with the same name (which is against
         * the spec 4.1 "A Vector Tile MUST NOT contain two or more layers
         * whose name values are byte-for-byte identical.") it is unspecified
         * which will be returned.
         *
         * @returns The specified layer or the invalid layer if there is no
         *          layer with this name.
         * @throws format_exception if the tile data is ill-formed.
         */
        layer operator[](const char* name) const {
            const data_view dv{name, std::strlen(name)};
            return (*this)[dv];
        }

    }; // class vector_tile

} // namespace vtzero

#endif // VTZERO_VECTOR_TILE_HPP
