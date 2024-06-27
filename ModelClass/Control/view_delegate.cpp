//
//  view_delegate.cpp
//  Test2
//
//  Created by Gustavo Binder on 10/06/24.
//

#include "view_delegate.hpp"

MyMTKViewDelegate::MyMTKViewDelegate( MTL::Device* pDevice )
: MTK::ViewDelegate()
, _pRenderer( new Renderer( pDevice ) )
{
//    Plane* plane = new Plane(pDevice);
//    mainNode = plane;
//    _pRenderer = new Renderer(pDevice, mainNode);
//    MTL::Tex
//    _pRenderer->importModel("Models/train.obj");
}

MyMTKViewDelegate::~MyMTKViewDelegate()
{
    delete _pRenderer;
//    delete mainNode;
}

void MyMTKViewDelegate::drawInMTKView( MTK::View* pView )
{
    _pRenderer->draw( pView );
}
