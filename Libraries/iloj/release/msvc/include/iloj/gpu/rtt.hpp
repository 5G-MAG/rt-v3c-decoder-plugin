/*
* Copyright (c) 2025 InterDigital CE Patent Holdings SASU
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

namespace iloj::gpu
{
template<typename T>
RenderToTexture<T>::RenderToTexture()
{
    m_frameBuffer.bind();

    m_frameBuffer.addColorAttachment<decltype(getAttachmentType(T()))>();
    m_frameBuffer.setDepthAttachment<decltype(getAttachmentType(T()))>();

    FrameBuffer::unbind();
}

template<typename T>
void RenderToTexture<T>::attach(const T &target, unsigned attachmentId)
{
    m_frameBuffer.bind();

    while (m_frameBuffer.getNumberOfColorAttachment() <= attachmentId)
    {
        m_frameBuffer.addColorAttachment<decltype(getAttachmentType(T()))>();
    }

    m_frameBuffer.getColorAttachment<decltype(getAttachmentType(T()))>(attachmentId).set(target);

    FrameBuffer::unbind();
}

template<typename T>
void RenderToTexture<T>::detach(unsigned attachmentId)
{
    m_frameBuffer.bind();

    while (m_frameBuffer.getNumberOfColorAttachment() <= attachmentId)
    {
        m_frameBuffer.addColorAttachment<decltype(getAttachmentType(T()))>();
    }

    m_frameBuffer.getColorAttachment<decltype(getAttachmentType(T()))>(attachmentId).clear();

    FrameBuffer::unbind();
}

template<typename T>
void RenderToTexture<T>::execute(T &target, DepthBufferType &depth, const std::function<void()> &plan)
{
    bool hasDepth = (isDepthMaskEnabled() || (glIsEnabled(GL_DEPTH_TEST) != 0U));

    m_frameBuffer.bind();

    if (hasDepth)
    {
        reshapeDepthBuffer(target, depth);
        m_frameBuffer.getDepthAttachment<decltype(getAttachmentType(DepthBufferType()))>().set(depth);
    }

    m_frameBuffer.getColorAttachment<decltype(getAttachmentType(T()))>().set(target);

    FrameBuffer::setDrawBuffers({0});

    plan();

    FrameBuffer::disableDrawBuffers();

    if (hasDepth)
    {
        m_frameBuffer.getDepthAttachment<decltype(getAttachmentType(DepthBufferType()))>().clear();
    }

    m_frameBuffer.getColorAttachment<decltype(getAttachmentType(T()))>().clear();

    FrameBuffer::unbind();
}

template<typename T>
void RenderToTexture<T>::execute(TargetList<T> targets, DepthBufferType &depth, const std::function<void()> &plan)
{
    bool hasDepth = (isDepthMaskEnabled() || (glIsEnabled(GL_DEPTH_TEST) != 0U));

    m_frameBuffer.bind();

    while (m_frameBuffer.getNumberOfColorAttachment() <= targets.size())
    {
        m_frameBuffer.addColorAttachment<decltype(getAttachmentType(T()))>();
    }

    if (hasDepth)
    {
        reshapeDepthBuffer(targets.begin()->get(), depth);
        m_frameBuffer.getDepthAttachment<decltype(getAttachmentType(DepthBufferType()))>().set(depth);
    }

    std::vector<unsigned> drawBuffers;

    for (auto &v : targets)
    {
        unsigned i = drawBuffers.size();
        drawBuffers.push_back(i);
        m_frameBuffer.getColorAttachment<decltype(getAttachmentType(T()))>(i).set(v.get());
    }

    FrameBuffer::setDrawBuffers(drawBuffers);

    plan();

    FrameBuffer::disableDrawBuffers();

    if (hasDepth)
    {
        m_frameBuffer.getDepthAttachment<decltype(getAttachmentType(DepthBufferType()))>().clear();
    }

    for (std::size_t i = 0U; i < targets.size(); i++)
    {
        m_frameBuffer.getColorAttachment<decltype(getAttachmentType(T()))>(i).clear();
    }

    FrameBuffer::unbind();
}
} // namespace iloj::gpu
