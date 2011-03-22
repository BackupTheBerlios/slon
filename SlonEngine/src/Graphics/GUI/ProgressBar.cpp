#include "Graphics/GUI/ProgressBar.h"

using namespace std;
using namespace sgl;
using namespace math;

using namespace slon;
using namespace slon::graphics;
using namespace gui;

ProgressBar::ProgressBar(sgl::Device* _pDevice) :
    pDevice(_pDevice),
    topLeft(0, 0),
    size(100, 20),
    color(1.0f, 1.0f, 1.0f, 0.5f),
    borderColor(1.0f, 1.0f, 0.0f, 1.0f),
    value(0)
{
    vbo.reset( pDevice->CreateVertexBuffer() );
    blendState.reset( pDevice->CreateBlendState() );
    colorState.reset( pDevice->CreateVertexColorState() );
    projectionTransform.reset( pDevice->CreateProjectionMatrixState() );
    modelviewTransform.reset( pDevice->CreateModelViewMatrixState() );
    depthTestState.reset( pDevice->CreateDepthState() );

    stateSet.Add( colorState.get() );
    stateSet.Add( blendState.get() );
    stateSet.Add( projectionTransform.get() );
    stateSet.Add( modelviewTransform.get() );
    stateSet.Add( depthTestState.get() );

    // customize states
    colorState->SetColor(color);
    blendState->SetBlendEquation(BlendState::ADD);
    blendState->SetSrcBlendFunc(BlendState::SRC_ALPHA);
    blendState->SetDstBlendFunc(BlendState::ONE_MINUS_SRC_ALPHA);
    depthTestState->ToggleDepthTest(false);

    rectangle vp = pDevice->Viewport();
    projectionTransform->SetMatrix(
        Mat4f::ortho(0.0f, (float)vp.width, (float)vp.height, 0.0f, -1.0f, 1.0f)
    );

    // create geometry
    std::vector<Vector2f> vertices;
    vertices.push_back( Vector2f( 0.0f, 0.0f) );
    vertices.push_back( Vector2f( 0.0f, 1.0f) );
    vertices.push_back( Vector2f( 1.0f, 1.0f) );
    vertices.push_back( Vector2f( 1.0f, 0.0f) );

	ref_ptr<VertexDeclaration> vDecl( pDevice->CreateVertexDeclaration() );
	vDecl->AddVertex(2, sgl::FLOAT);

    vbo->SetVertexDecl( vDecl.get() );
	vbo->SetData( vertices.size() * sizeof(Vector2f), &vertices[0] );
}

ProgressBar::~ProgressBar()
{
    //dtor
}

void ProgressBar::SetPosition(int x, int y)
{
    topLeft = Vector2i(x, y);
}

void ProgressBar::SetSize(int width, int height)
{
    size = Vector2i(width, height);
}

/** Set color of the button
 * @param color of the button. The last parameter is opacity
 */
void ProgressBar::SetColor(float r, float g, float b, float a)
{
    this->color = Vector4f(r, g, b, a);
}

/** Color of the border */
void ProgressBar::SetBorderColor(float r, float g, float b, float a)
{
    borderColor = Vector4f(r, g, b, a);
}

/** Set font of the button */
void ProgressBar::SetFont(sgl::Font* _font)
{
    font.reset(_font);
}

/** Set text of the progresss bar */
void ProgressBar::SetText(const std::string& text)
{
    this->text = text;
}

/** Set the value of the progress bar in the [0,1] segment */
void ProgressBar::SetValue(float value)
{
    this->value = value;
}

void ProgressBar::Draw() const
{
    // prepares states
    ref_const_cast<VertexColorState>(colorState)->SetColor(color);
    ref_const_cast<TransformState>(modelviewTransform)->SetMatrix(
        Mat4f::scale( (float)size.x * value, (float)size.y, 1.0f)
        * Mat4f::translate( (float)topLeft.x, (float)topLeft.y, 0.0f)
    );

    vbo->Bind();

    stateSet.Begin();
    vbo->Draw(QUADS);

    // prepare states
    ref_const_cast<VertexColorState>(colorState)->SetColor(borderColor);
    ref_const_cast<TransformState>(modelviewTransform)->SetMatrix(
        Mat4f::scale( (float)size.x, (float)size.y, 1.0f )
        * Mat4f::translate( (float)topLeft.x, (float)topLeft.y, 0.0f )
    );
    colorState->Setup();
    modelviewTransform->Setup();

    vbo->Draw(LINE_LOOP);
    stateSet.End();

    vbo->Unbind();

    // draw text
    if (font)
    {
        int textX = -static_cast<int>(10 * text.size()) - 4;
        int textY = (size.y - 12) / 2;

        font->Bind(10, 12);
        font->Print( (float)topLeft.x + textX,
                     (float)topLeft.y + textY,
                     text );
        font->Unbind();
    }
}
