//
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

/** Descriptor Set wrapper implementation.
 *
 *  Implemented to:
 *
 *  - reference-count wrapper instances
 *  - cache set binding information.
 *  - monitor layout adjustments and act accordingly.
 *  - monitor pool reset events and act accordingly.
 *
 *  Not thread-safe at the moment.
 */
#ifndef WRAPPERS_DESCRIPTOR_SET_H
#define WRAPPERS_DESCRIPTOR_SET_H

#include "misc/debug_marker.h"
#include "misc/mt_safety.h"
#include "misc/types.h"

namespace Anvil
{
    class DescriptorSet : public DebugMarkerSupportProvider<DescriptorSet>,
                          public MTSafetySupportProvider
    {
    public:
        /** Represents a single buffer object, which can be bound to a specific descriptor set slot */
        typedef struct BufferBindingElement
        {
            std::shared_ptr<Anvil::Buffer> buffer_ptr;
            VkDeviceSize                   size;
            VkDeviceSize                   start_offset;

            /** Constructor. Associates all available buffer memory with the binding.
             *
             *  @param in_buffer_ptr Buffer object to use for the binding. Must not be nullptr.
             **/
            BufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr);

            /** Constructor. Associates specified sub-region of the buffer memory with the binding.
             *
             *  @param in_buffer_ptr Buffer object to use for the binding. Must not be nullptr.
             **/
            BufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr,
                                 VkDeviceSize                   in_start_offset,
                                 VkDeviceSize                   in_size);

            /** Destructor. Releases the encapsulated buffer instance */
            virtual ~BufferBindingElement();

            /** Copy assignment operator.
             *
             *  Retains the buffer object embedded in @param in.
             **/
            BufferBindingElement(const BufferBindingElement& in);

            /* Returns Vulkan descriptor type for this structure */
            virtual VkDescriptorType get_type() const = 0;

        private:
            BufferBindingElement& operator=(const BufferBindingElement& in);
        } BufferBindingElement;

        /** Holds a single buffer instance. Can be used to bind the object to a descriptor set slot
         *  as a dynamic storage buffer.
         **/
        struct DynamicStorageBufferBindingElement : public BufferBindingElement
        {
            DynamicStorageBufferBindingElement() = delete;

            DynamicStorageBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr)
                : BufferBindingElement(in_buffer_ptr)
            {
                /* Stub */
            }

            DynamicStorageBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr,
                                               VkDeviceSize                   in_start_offset,
                                               VkDeviceSize                   in_size)
                : BufferBindingElement(in_buffer_ptr,
                                       in_start_offset,
                                       in_size)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            }
        };

        /** Holds a single buffer instance. Can be used to bind the object to a descriptor set slot
         *  as a dynamic uniform buffer.
         **/
        struct DynamicUniformBufferBindingElement : public BufferBindingElement
        {
            DynamicUniformBufferBindingElement() = delete;

            DynamicUniformBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr)
                : BufferBindingElement(in_buffer_ptr)
            {
                /* Stub */
            }

            DynamicUniformBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr,
                                               VkDeviceSize                   in_start_offset,
                                               VkDeviceSize                   in_size)
                : BufferBindingElement(in_buffer_ptr,
                                       in_start_offset,
                                       in_size)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            }
        };

        /** Holds a single buffer instance. Can be used to bind the object to a descriptor set slot
         *  as a storage buffer.
         **/
        struct StorageBufferBindingElement : public BufferBindingElement
        {
            StorageBufferBindingElement() = delete;

            StorageBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr)
                :BufferBindingElement(in_buffer_ptr)
            {
                /* Stub */
            }

            StorageBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr,
                                        VkDeviceSize                   in_start_offset,
                                        VkDeviceSize                   in_size)
                :BufferBindingElement(in_buffer_ptr,
                                      in_start_offset,
                                      in_size)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            }
        };

        /** Holds a single buffer instance. Can be used to bind the object to a descriptor set slot
         *  as a uniform buffer.
         **/
        struct UniformBufferBindingElement : BufferBindingElement
        {
            UniformBufferBindingElement() = delete;

            UniformBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr)
                :BufferBindingElement(in_buffer_ptr)
            {
                /* Stub */
            }

            UniformBufferBindingElement(std::shared_ptr<Anvil::Buffer> in_buffer_ptr,
                                        VkDeviceSize                   in_start_offset,
                                        VkDeviceSize                   in_size)
                :BufferBindingElement(in_buffer_ptr,
                                      in_start_offset,
                                      in_size)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }
        };

        /** Holds a single combined image+sampler pair, along with other metadata required to bind the two
         *  to a specific descriptor set slot as a combined image+sampler
         **/
        typedef struct CombinedImageSamplerBindingElement
        {
            VkImageLayout                     image_layout;
            std::shared_ptr<Anvil::ImageView> image_view_ptr; 
            std::shared_ptr<Anvil::Sampler>   sampler_ptr;

            /** Constructor.
             *
             *  @param in_image_layout   Image layout to use for the binding.
             *  @param in_image_view_ptr Image view to use for the binding. Must not be nullptr.
             *  @param in_sampler_ptr    Sampler to use for the binding. Can be nullptr, in which case
             *                           it will be assumed the element corresponds to an immutable
             *                           sampler.
             **/
            CombinedImageSamplerBindingElement(VkImageLayout                     in_image_layout,
                                               std::shared_ptr<Anvil::ImageView> in_image_view_ptr,
                                               std::shared_ptr<Anvil::Sampler>   in_sampler_ptr);

            /** Destructor.
             *
             *  Releases the embedded image view & sampler instances.
             **/
            ~CombinedImageSamplerBindingElement();

            /** Copy assignment operator.
             *
             *  Retains the image view and sampler objects embedded in @param in.
             **/
            CombinedImageSamplerBindingElement(const CombinedImageSamplerBindingElement& in);

            /* Returns Vulkan descriptor type for this structure */
            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            }

        private:
            CombinedImageSamplerBindingElement& operator=(const CombinedImageSamplerBindingElement&);
        } CombinedImageSamplerBindingElement;

        /** Holds a single image view, along with other metadata required bound it to a specific descriptor set slot */
        typedef struct ImageBindingElement
        {
            VkImageLayout                     image_layout;
            std::shared_ptr<Anvil::ImageView> image_view_ptr;

            /** Constructor.
             *
             *  @param in_image_layout   Image layout to use for the binding.
             *  @param in_image_view_ptr Image view to use for the binding. Must not be nullptr.
             **/
            ImageBindingElement(VkImageLayout                     in_image_layout,
                                std::shared_ptr<Anvil::ImageView> in_image_view_ptr);

            /** Copy assignment operator.
             *
             *  Retains the image view embedded in @param in.
             **/
            ImageBindingElement(const ImageBindingElement& in);

            /** Destructor.
             *
             *  Releases the embedded image view instance.
             **/
            ~ImageBindingElement();

            /* Returns Vulkan descriptor type for this structure */
            virtual VkDescriptorType get_type() const = 0;

        private:
            ImageBindingElement& operator=(const ImageBindingElement&);
        } ImageBindingElement;

        /** Holds a single image view, along with other metadata required to bound it to a specific
         *  descriptor set slot as an input attachment
         **/
        struct InputAttachmentBindingElement : public ImageBindingElement
        {
            InputAttachmentBindingElement() = delete;

            InputAttachmentBindingElement(VkImageLayout                     in_image_layout,
                                          std::shared_ptr<Anvil::ImageView> in_image_view_ptr)
                :ImageBindingElement(in_image_layout,
                                     in_image_view_ptr)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            }
        };

        /** Holds a single image view, along with other metadata required to bound it to a specific
         *  descriptor set slot as a sampled image.
         **/
        struct SampledImageBindingElement : ImageBindingElement
        {
            SampledImageBindingElement() = delete;

            SampledImageBindingElement(VkImageLayout                     in_image_layout,
                                       std::shared_ptr<Anvil::ImageView> in_image_view_ptr)
                :ImageBindingElement(in_image_layout,
                                     in_image_view_ptr)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            }
        };

        /** Holds a single image view, along with other metadata required to bound it to a specific
         *  descriptor set slot as a storage image.
         **/
        struct StorageImageBindingElement : public ImageBindingElement
        {
            StorageImageBindingElement() = delete;

            StorageImageBindingElement(VkImageLayout                     in_image_layout,
                                       std::shared_ptr<Anvil::ImageView> in_image_view_ptr)
                :ImageBindingElement(in_image_layout,
                                     in_image_view_ptr)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            }
        };

        /** Holds a single sampler. Can be used to bind a sampler to a descriptor set slot **/
        typedef struct SamplerBindingElement
        {
            std::shared_ptr<Anvil::Sampler> sampler_ptr;

            /** Constructor.
             *
             *  @param in_sampler_ptr Sampler to use for the binding. Can be nullptr, in which case
             *                        it will be assumed the element corresponds to an immutable
             *                        sampler.
             **/
            SamplerBindingElement(std::shared_ptr<Anvil::Sampler> in_sampler_ptr);

            /** Destructor.
             *
             *  Releases the embedded sampler instance.
             **/
            ~SamplerBindingElement();

            /** Copy assignment operator.
             *
             *  Retains the sampler embedded in @param in.
             **/
            SamplerBindingElement(const SamplerBindingElement& in);

            /* Returns Vulkan descriptor type for this structure */
            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_SAMPLER;
            }

        private:
            SamplerBindingElement& operator=(const SamplerBindingElement&);
        } SamplerBindingElement;

        /** Holds a single buffer view instance. Can be used to bind a sampler to a descriptor set slot */
        typedef struct TexelBufferBindingElement
        {
            std::shared_ptr<Anvil::BufferView> buffer_view_ptr;

            /** Constructor.
             *
             *  @param in_buffer_view_ptr Buffer view to use for the binding. Must not be nullptr.
             *                            Retained. The object will be released at destruction time.
             **/
            TexelBufferBindingElement(std::shared_ptr<Anvil::BufferView> in_buffer_view_ptr);

            /** Destructor.
             *
             *  Releases the embedded buffer view instance.
             **/
            ~TexelBufferBindingElement();

            /** Copy assignment operator.
             *
             *  Retains the buffer view embedded in @param in.
             **/
            TexelBufferBindingElement(const TexelBufferBindingElement& in);

            /* Returns Vulkan descriptor type for this structure */
            virtual VkDescriptorType get_type() const = 0;

        private:
            TexelBufferBindingElement& operator=(const TexelBufferBindingElement&);
        } TexelBufferBindingElement;

        /** Holds a single buffer view instance. Can be used to bind a sampler to a descriptor set slot
         *  as a storage texel buffer.
         **/
        struct StorageTexelBufferBindingElement : public TexelBufferBindingElement
        {
            StorageTexelBufferBindingElement() = delete;

            StorageTexelBufferBindingElement(std::shared_ptr<Anvil::BufferView> in_buffer_view_ptr)
                : TexelBufferBindingElement(in_buffer_view_ptr)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            }
        };

        /** Holds a single buffer view instance. Can be used to bind a sampler to a descriptor set slot
         *  as a uniform texel buffer.
         **/
        struct UniformTexelBufferBindingElement : public TexelBufferBindingElement
        {
            UniformTexelBufferBindingElement() = delete;

            UniformTexelBufferBindingElement(std::shared_ptr<Anvil::BufferView> in_buffer_view_ptr)
                : TexelBufferBindingElement(in_buffer_view_ptr)
            {
                /* Stub */
            }

            VkDescriptorType get_type() const
            {
                return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            }
        };


        /* Public functions */

        /** Destructor.
         *
         *  Unregisters the object from the object tracker.
         **/
        virtual ~DescriptorSet();

        /** Updates internally-maintained Vulkan descriptor set instances.
         *
         *  @return true if the function executed successfully, false otherwise.
         **/
        bool bake();

        /** Creates a new DescriptorSet instance.
         *
         *  @param in_device_ptr      Device the descriptor set has been initialized for.
         *  @param in_parent_pool_ptr Pool, from which the descriptor set has been allocated from.
         *                            Must not be nullptr.
         *  @param in_layout_ptr      Layout which has been used at descriptor set construction time.
         *  @param in_descriptor_set  Raw Vulkan handle the wrapper instance is being created for.
         **/
        static std::shared_ptr<DescriptorSet> create(std::weak_ptr<Anvil::BaseDevice>            in_device_ptr,
                                                     std::shared_ptr<Anvil::DescriptorPool>      in_parent_pool_ptr,
                                                     std::shared_ptr<Anvil::DescriptorSetLayout> in_layout_ptr,
                                                     VkDescriptorSet                             in_descriptor_set,
                                                     MTSafety                                    in_mt_safety = MT_SAFETY_INHERIT_FROM_PARENT_DEVICE);

        /** Tells how many array items have been declared for a binding at a given index
         *
         *  @param in_n_binding   Binding index to use for the query.
         *  @param out_result_ptr If the function reports success, deref will be set to the query result.
         *                        Must not be null.
         *
         *  @return true if successful, false otherwise.
         **/
        bool get_binding_array_size(uint32_t  in_n_binding,
                                    uint32_t* out_result_ptr) const;

        /** Tells the descriptor type associated to a binding at index @param in_n_binding .
         *
         *  @param in_n_binding            Binding index to use for the query.
         *  @param out_descriptor_type_ptr If the function reports success, deref will be set to the query result.
         *                                 Must not be null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_binding_descriptor_type(uint32_t          in_n_binding,
                                         VkDescriptorType* out_descriptor_type_ptr) const;

        /** Returns properties of a combined image/sampler descriptor binding.
         *
         *  @param in_n_binding             Binding index to use for the query.
         *  @param in_n_binding_array_item  Index of the array item to use for the query.
         *  @param out_opt_image_layout_ptr If the function reports success, deref will be set to the image layout
         *                                  specified for the binding. May be null.
         *  @param out_opt_image_view_ptr   If the function reports success, deref will be set to the image view
         *                                  specified for the binding. May be null.
         *  @param out_opt_sampler_ptr      If the function reports success, deref will be set to the sampler specified
         *                                  for the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         **/
        bool get_combined_image_sampler_binding_properties(uint32_t                           in_n_binding,
                                                           uint32_t                           in_n_binding_array_item,
                                                           VkImageLayout*                     out_opt_image_layout_ptr,
                                                           std::shared_ptr<Anvil::ImageView>* out_opt_image_view_ptr,
                                                           std::shared_ptr<Anvil::Sampler>*   out_opt_sampler_ptr);

        /** Retrieves raw Vulkan handle of the encapsulated descriptor set.
         *
         *  If the wrapper instance is marked as dirty, the function will bake the descriptor set,
         *  prior to returning the handle.
         *
         *  @return As per description.
         **/
        VkDescriptorSet get_descriptor_set_vk()
        {
            if (m_dirty)
            {
                bake();

                anvil_assert(!m_dirty);
            }

            return m_descriptor_set;
        }

        /** Returns a descriptor set layout wrapper instance, assigned to the descriptor set wrapper */
        std::shared_ptr<Anvil::DescriptorSetLayout> get_descriptor_set_layout() const
        {
            return m_layout_ptr;
        }

        /** Returns properties of an input attachment descriptor binding.
         *
         *  @param in_n_binding             Binding index to use for the query.
         *  @param in_n_binding_array_item  Index of the array item to use for the query.
         *  @param out_opt_image_layout_ptr If not null, deref will be set to the image layout specified for
         *                                  the binding. May be null.
         *  @param out_opt_image_view_ptr   If not null, deref will be set to the image view specified for
         *                                  the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         *
         */
        bool get_input_attachment_binding_properties(uint32_t                           in_n_binding,
                                                     uint32_t                           in_n_binding_array_item,
                                                     VkImageLayout*                     out_opt_image_layout_ptr,
                                                     std::shared_ptr<Anvil::ImageView>* out_opt_image_view_ptr) const;

        /** Returns information about the number of bindings described by the descriptor set. */
        uint32_t get_n_bindings() const
        {
            return static_cast<uint32_t>(m_bindings.size() );
        }

        /** Returns properties of a sampled image descriptor binding.
         *
         *  @param in_n_binding             Binding index to use for the query.
         *  @param in_n_binding_array_item  Index of the array item to use for the query.
         *  @param out_opt_image_layout_ptr If not null, deref will be set to the image layout specified
         *                                  for the binding. May be null.
         *  @param out_opt_image_view_ptr   If not null, deref will be set to the image view specified for
         *                                  the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_sampled_image_binding_properties(uint32_t                           in_n_binding,
                                                  uint32_t                           in_n_binding_array_item,
                                                  VkImageLayout*                     out_opt_image_layout_ptr,
                                                  std::shared_ptr<Anvil::ImageView>* out_opt_image_view_ptr) const
        {
            /* Re-use existing code */
            return get_input_attachment_binding_properties(in_n_binding,
                                                           in_n_binding_array_item,
                                                           out_opt_image_layout_ptr,
                                                           out_opt_image_view_ptr);
        }

        /** Returns properties of a sampler descriptor binding.
         *
         *  @param in_n_binding            Binding index to use for the query.
         *  @param in_n_binding_array_item Index of the array item to use for the query.
         *  @param out_opt_sampler_ptr     If not null, deref will be set to the sampler specified for
         *                                 the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_sampler_binding_properties(uint32_t                         in_n_binding,
                                            uint32_t                         in_n_binding_array_item,
                                            std::shared_ptr<Anvil::Sampler>* out_sampler_ptr) const;

        /** Returns properties of a storage buffer descriptor binding.
         *
         *  @param in_n_binding             Binding index to use for the query.
         *  @param in_n_binding_array_item  Index of the array item to use for the query.
         *  @param out_opt_buffer_ptr       If not null, deref will be set to the buffer specified for
         *                                  the binding. May be null.
         *  @param out_opt_size_ptr         If not null, deref will be set to the size of the buffer
         *                                  memory region associated with the binding. May be null.
         *  @param out_opt_start_offset_ptr If not null, deref will be set to the start offset of the
         *                                  buffer memory region associated with the binding. May be
         *                                  null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_storage_buffer_binding_properties(uint32_t                        in_n_binding,
                                                   uint32_t                        in_n_binding_array_item,
                                                   std::shared_ptr<Anvil::Buffer>* out_opt_buffer_ptr,
                                                   VkDeviceSize*                   out_opt_size_ptr,
                                                   VkDeviceSize*                   out_opt_start_offset_ptr) const;

        /** Returns properties of a storage image descriptor binding.
         *
         *  @param in_n_binding             Binding index to use for the query.
         *  @param in_n_binding_array_item  Index of the array item to use for the query.
         *  @param out_opt_image_layout_ptr If not null, deref will be set to the image layout declared for
         *                                  the binding. May be null.
         *  @param out_opt_image_view_ptr   If not null, deref will be set to the image view specified for
         *                                  the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_storage_image_binding_properties(uint32_t                           in_n_binding,
                                                  uint32_t                           in_n_binding_array_item,
                                                  VkImageLayout*                     out_opt_image_layout_ptr,
                                                  std::shared_ptr<Anvil::ImageView>* out_opt_image_view_ptr) const
        {
            /* Re-use existing code */
            return get_input_attachment_binding_properties(in_n_binding,
                                                           in_n_binding_array_item,
                                                           out_opt_image_layout_ptr,
                                                           out_opt_image_view_ptr);
        }

        /** Returns properties of a storage texel buffer descriptor binding.
         *
         *  @param in_n_binding            Binding index to use for the query.
         *  @param in_n_binding_array_item Index of the array item to use for the query.
         *  @param out_opt_buffer_view_ptr If not null, deref will be set to the buffer view specified for
         *                                 the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_storage_texel_buffer_binding_properties(uint32_t                            in_n_binding,
                                                         uint32_t                            in_n_binding_array_item,
                                                         std::shared_ptr<Anvil::BufferView>* out_opt_buffer_view_ptr) const;

        /** Returns properties of a uniform buffer descriptor binding.
         *
         *  @param in_n_binding             Binding index to use for the query.
         *  @param in_n_binding_array_item  Index of the array item to use for the query.
         *  @param out_opt_buffer_ptr       If not null, deref will be set to the buffer specified for
         *                                  the binding. May be null.
         *  @param out_opt_size_ptr         If not null, deref will be set to size of the buffer memory region
         *                                  declared for the binding. May be null.
         *  @param out_opt_start_offset_ptr If not null, deref will be set to start offset of the buffer memory
         *                                  region declared for the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_uniform_buffer_binding_properties(uint32_t                        in_n_binding,
                                                   uint32_t                        in_n_binding_array_item,
                                                   std::shared_ptr<Anvil::Buffer>* out_opt_buffer_ptr,
                                                   VkDeviceSize*                   out_opt_size_ptr,
                                                   VkDeviceSize*                   out_opt_start_offset_ptr) const
        {
            /* Re-use existing code */
            return get_storage_buffer_binding_properties(in_n_binding,
                                                         in_n_binding_array_item,
                                                         out_opt_buffer_ptr,
                                                         out_opt_size_ptr,
                                                         out_opt_start_offset_ptr);
        }

        /** Returns properties of a uniform texel buffer descriptor binding.
         *
         *  @param in_n_binding            Binding index to use for the query.
         *  @param in_n_binding_array_item Index of the array item to use for the query.
         *  @param out_opt_buffer_view_ptr If not null, deref will be set to the buffer view specified for
         *                                 the binding. May be null.
         *
         *  @return true if successful, false otherwise.
         */
        bool get_uniform_texel_buffer_binding_properties(uint32_t                            in_n_binding,
                                                         uint32_t                            in_n_binding_array_item,
                                                         std::shared_ptr<Anvil::BufferView>* out_opt_buffer_view_ptr) const
        {
            /* Re-use existing code */
            return get_storage_texel_buffer_binding_properties(in_n_binding,
                                                               in_n_binding_array_item,
                                                               out_opt_buffer_view_ptr);
        }

        /** This function should be set to assign physical Vulkan objects to a descriptor binding
         *  at index @param in_binding_index for descriptor set @param in_n_set.
         *  Each binding can hold one or more objects. Which slots the specified objects should take can
         *  be configured by passing the right values to @param in_element_range.
         *  Objects are passed via @param in_elements argument. The argument must be passed an object of
         *  one of the following types, depending on what object is to be attached to the specified
         *  descriptor binding:
         *
         *  CombinedImageSamplerBindingElement - for combined image+sampler bindings.
         *  DynamicStorageBufferBindingElement - for dynamic storage buffer bindings.
         *  DynamicUniformBufferBindingElement - for dynamic uniform buffer bindings.
         *  InputAttachmentBindingElement      - for input attachment bindings.
         *  SampledImageBindingElement         - for sampled image bindings.
         *  SamplerBindingElement              - for sampler bindings.
         *  StorageBufferBindingElement        - for storage buffer bindings.
         *  StorageImageBindingElement         - for storage image bindings.
         *  StorageTexelBufferBindingElement   - for storage texel buffer bindings. 
         *  UniformBufferBindingElement        - for uniform buffer bindings.
         *  UniformTexelBufferBindingElement   - for storage uniform buffer bindings.
         *
         *  @param in_binding_index As per documentation. Must correspond to a binding which has earlier
         *                          been added by calling add_binding() function.
         *  @param in_element_range As per documentation. Must not be equal to or larger than the array size,
         *                          specified when calling add_binding() function.
         *  @param in_elements      As per documentation. Must not be nullptr.
         *
         *  @return true if the function executed successfully, false otherwise.
         **/
        template<typename BindingElementType>
        bool set_binding_array_items(BindingIndex              in_binding_index,
                                     BindingElementArrayRange  in_element_range,
                                     const BindingElementType* in_elements)
        {
            anvil_assert(in_elements != nullptr);
            anvil_assert(m_unusable   == false);

            BindingItems&  binding_items      = m_bindings[in_binding_index];
            const uint32_t last_element_index = in_element_range.second + in_element_range.first;

            for (BindingElementIndex current_element_index = in_element_range.first;
                                     current_element_index < last_element_index;
                                   ++current_element_index)
            {
                m_dirty |= !(binding_items[current_element_index] == in_elements[current_element_index - in_element_range.first]);

                binding_items[current_element_index] = in_elements[current_element_index - in_element_range.first];
            }

            return true;
        }

        /** This function works exactly like set_binding_array_items(), except that it always replaces the zeroth element
         *  attached to the specified descriptor set's binding.
         */
        template<typename BindingElementType>
        bool set_binding_item(BindingIndex              in_binding_index,
                              const BindingElementType& in_element)
        {
            return set_binding_array_items(in_binding_index,
                                           BindingElementArrayRange(0,  /* StartBindingElementIndex */
                                                                    1), /* NumberOfBindingElements  */
                                          &in_element);
        }

        /** Assigns a new Vulkan descriptor set handle to the wrapper instance.
         *
         *  This function should only be used internally. Its purpose is to introduce support for "recycling" of
         *  deprecated descriptor sets. When a descriptor set pool is reset or the descriptor set layout is adjusted,
         *  Descriptor Set's Vulkan handle may become obsolete. When that happens, this function can be called to
         *  "revive" the object by assigning it a new handle, at which point the object becomes usable again.
         *  Furthermore, all cached binding information will be automatically writtne to the descriptor set at
         *  next baking time.
         *
         *  @param in_ds New Vulkan handle to use. Must not be VK_NULL_HANDLE.
         **/
        void set_new_vk_handle(VkDescriptorSet in_ds);

    private:
        /* Private type declarations */

        /** Structure which holds raw Vulkan objects, required to perform a write op against
         *  a descriptor set.
         *
         *  Each structure instance is assumed to describe a single binding's array item.
         **/
        typedef struct BindingItem
        {
            std::shared_ptr<Anvil::Buffer>     buffer_ptr;
            std::shared_ptr<Anvil::BufferView> buffer_view_ptr;
            VkImageLayout                      image_layout;
            std::shared_ptr<Anvil::ImageView>  image_view_ptr;
            std::shared_ptr<Anvil::Sampler>    sampler_ptr;
            VkDeviceSize                       size;
            VkDeviceSize                       start_offset;
            VkDescriptorType                   type_vk;

            bool dirty;

            bool operator==(const BufferBindingElement& in) const
            {
                return (buffer_ptr   == in.buffer_ptr      &&
                        size         == in.size            &&
                        start_offset == in.start_offset);
            }

            bool operator==(const CombinedImageSamplerBindingElement& in) const
            {
                return (image_layout   == in.image_layout     &&
                        image_view_ptr == in.image_view_ptr   &&
                        sampler_ptr    == in.sampler_ptr);
            }

            bool operator==(const ImageBindingElement& in) const
            {
                return (image_layout   == in.image_layout   &&
                        image_view_ptr == in.image_view_ptr);
            }

            bool operator==(const SamplerBindingElement& in) const
            {
                return (sampler_ptr == in.sampler_ptr);
            }

            bool operator==(const TexelBufferBindingElement& in) const
            {
                return (buffer_view_ptr == in.buffer_view_ptr);
            }

            /* Copy assignment operator.
             *
             * Retains the buffer instance embedded in @param in_element
             **/
            BindingItem& operator=(const BufferBindingElement& in_element);

            /* Copy assignment operator.
             *
             * Retains the image view & sampler instances embedded in @param in_element
             **/
            BindingItem& operator=(const CombinedImageSamplerBindingElement& in_element);

            /* Copy assignment operator.
             *
             * Retains the image view instance embedded in @param in_element
             **/
            BindingItem& operator=(const ImageBindingElement& in_element);

            /* Copy assignment operator.
             *
             * Retains the sampler instance embedded in @param in_element
             **/
            BindingItem& operator=(const SamplerBindingElement& in_element);

            /* Copy assignment operator.
             *
             * Retains the buffer view instance embedded in @param in_element
             **/
            BindingItem& operator=(const TexelBufferBindingElement& in_element);

            /* Default dummy constructor. */ 
            BindingItem()
            {
                dirty        = false;
                image_layout = VK_IMAGE_LAYOUT_MAX_ENUM;
                size         = 0;
                start_offset = 0;
            }

            /* Destructor.
             *
             * Releases all wrapper instances embedded in the descriptor */
            ~BindingItem();
        } BindingItem;

        typedef std::vector<BindingItem>             BindingItems;
        typedef std::map<BindingIndex, BindingItems> BindingIndexToBindingItemsMap;

        /* Private functions */

        /** Please see create() documentation for argument discussion */
        DescriptorSet(std::weak_ptr<Anvil::BaseDevice>            in_device_ptr,
                      std::shared_ptr<Anvil::DescriptorPool>      in_parent_pool_ptr,
                      std::shared_ptr<Anvil::DescriptorSetLayout> in_layout_ptr,
                      VkDescriptorSet                             in_descriptor_set,
                      bool                                        in_mt_safe);

        DescriptorSet           (const DescriptorSet&);
        DescriptorSet& operator=(const DescriptorSet&);

        void on_parent_pool_reset();

        void alloc_bindings();

        /* Private variables */
        BindingIndexToBindingItemsMap               m_bindings;
        VkDescriptorSet                             m_descriptor_set;
        std::weak_ptr<Anvil::BaseDevice>            m_device_ptr;
        bool                                        m_dirty;
        std::shared_ptr<Anvil::DescriptorSetLayout> m_layout_ptr;
        bool                                        m_unusable;
        std::shared_ptr<Anvil::DescriptorPool>      m_parent_pool_ptr;

        std::vector<VkDescriptorBufferInfo> m_cached_ds_info_buffer_info_items_vk;
        std::vector<VkDescriptorImageInfo>  m_cached_ds_info_image_info_items_vk;
        std::vector<VkBufferView>           m_cached_ds_info_texel_buffer_info_items_vk;
        std::vector<VkWriteDescriptorSet>   m_cached_ds_write_items_vk;
    };
};

#endif /* WRAPPERS_DESCRIPTOR_SET_H */