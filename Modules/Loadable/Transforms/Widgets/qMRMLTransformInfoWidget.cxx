/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Andras Lasso and Franklin King at
  PerkLab, Queen's University and was supported through the Applied Cancer
  Research Unit program of Cancer Care Ontario with funds provided by the
  Ontario Ministry of Health and Long-Term Care.

==============================================================================*/


// qMRML includes
#include "qMRMLTransformInfoWidget.h"
#include "ui_qMRMLTransformInfoWidget.h"

// MRML includes
#include <vtkMRMLTransformNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>

// QSlicer includes
#include "qSlicerLayoutManager.h"
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"

// VTK includes
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorObserver.h>
#include <vtkTransform.h>


//------------------------------------------------------------------------------
class qMRMLTransformInfoWidgetPrivate: public Ui_qMRMLTransformInfoWidget
{
  Q_DECLARE_PUBLIC(qMRMLTransformInfoWidget);

protected:
  qMRMLTransformInfoWidget* const q_ptr;

public:
  qMRMLTransformInfoWidgetPrivate(qMRMLTransformInfoWidget& object);
  void init();
  void resetLabels();
  qMRMLSliceWidget* slicerWidget(vtkInteractorObserver * interactorStyle) const;
  QList<vtkInteractorObserver*> currentLayoutSliceViewInteractorStyles() const;

  vtkMRMLTransformNode* MRMLTransformNode;
  qSlicerLayoutManager* LayoutManager;
  QList<vtkInteractorObserver*> ObservedInteractorStyles;
  };

//------------------------------------------------------------------------------
qMRMLTransformInfoWidgetPrivate
::qMRMLTransformInfoWidgetPrivate(qMRMLTransformInfoWidget& object)
  : q_ptr(&object), LayoutManager(0)
{
  this->MRMLTransformNode = 0;
}

//------------------------------------------------------------------------------
void qMRMLTransformInfoWidgetPrivate::init()
{
  Q_Q(qMRMLTransformInfoWidget);
  this->setupUi(q);
  q->setEnabled(this->MRMLTransformNode != 0);
  this->TransformToParentInfoTextBrowser->setTextInteractionFlags(Qt::TextSelectableByMouse);
  this->TransformFromParentInfoTextBrowser->setTextInteractionFlags(Qt::TextSelectableByMouse);
  this->resetLabels();
}

//-----------------------------------------------------------------------------
void qMRMLTransformInfoWidgetPrivate::resetLabels()
{
  this->ViewerDisplacementVectorRAS->clear();
}

//-----------------------------------------------------------------------------
qMRMLSliceWidget*
qMRMLTransformInfoWidgetPrivate
::slicerWidget(vtkInteractorObserver* interactorStyle) const
{
  if (!this->LayoutManager)
    {
    return 0;
    }
  foreach(const QString& sliceViewName, this->LayoutManager->sliceViewNames())
    {
    qMRMLSliceWidget* sliceWidget = this->LayoutManager->sliceWidget(sliceViewName);
    Q_ASSERT(sliceWidget);
    if (sliceWidget->sliceView()->interactorStyle() == interactorStyle)
      {
      return sliceWidget;
      }
    }
  return 0;
}

//-----------------------------------------------------------------------------
QList<vtkInteractorObserver*>
qMRMLTransformInfoWidgetPrivate::currentLayoutSliceViewInteractorStyles() const
{
  QList<vtkInteractorObserver*> interactorStyles;
  if (!this->LayoutManager)
    {
    return interactorStyles;
    }
  foreach(const QString& sliceViewName, this->LayoutManager->sliceViewNames())
    {
    qMRMLSliceWidget * sliceWidget = this->LayoutManager->sliceWidget(sliceViewName);
    Q_ASSERT(sliceWidget);
    interactorStyles << sliceWidget->sliceView()->interactorStyle();
    }
  return interactorStyles;
}

//------------------------------------------------------------------------------
qMRMLTransformInfoWidget::qMRMLTransformInfoWidget(QWidget *_parent)
  : Superclass(_parent)
  , d_ptr(new qMRMLTransformInfoWidgetPrivate(*this))
{
  Q_D(qMRMLTransformInfoWidget);
  d->init();
}

//------------------------------------------------------------------------------
qMRMLTransformInfoWidget::~qMRMLTransformInfoWidget()
{
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(qMRMLTransformInfoWidget, qSlicerLayoutManager*, layoutManager, LayoutManager)

//-----------------------------------------------------------------------------
void qMRMLTransformInfoWidget::setLayoutManager(
  qSlicerLayoutManager* layoutManager)
{
  Q_D(qMRMLTransformInfoWidget);

  if (layoutManager == d->LayoutManager)
    {
    return;
    }
  if (d->LayoutManager)
    {
    disconnect(d->LayoutManager, SIGNAL(layoutChanged()),
      this, SLOT(onLayoutChanged()));
    }
  if (layoutManager)
    {
    connect(layoutManager, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));
    }
  d->LayoutManager = layoutManager;

  this->onLayoutChanged();
}

//-----------------------------------------------------------------------------
void qMRMLTransformInfoWidget::onLayoutChanged()
{
  Q_D(qMRMLTransformInfoWidget);

  // Remove observers
  foreach(vtkInteractorObserver * observedInteractorStyle, d->ObservedInteractorStyles)
    {
    foreach(int event, QList<int>() << vtkCommand::MouseMoveEvent
      << vtkCommand::EnterEvent << vtkCommand::LeaveEvent)
      {
      qvtkDisconnect(observedInteractorStyle, event,
                     this, SLOT(processEvent(vtkObject*,void*,ulong,void*)));
      }
    }
  d->ObservedInteractorStyles.clear();

  // Add observers
  foreach(vtkInteractorObserver * interactorStyle, d->currentLayoutSliceViewInteractorStyles())
    {
    foreach(int event, QList<int>() << vtkCommand::MouseMoveEvent
      << vtkCommand::EnterEvent << vtkCommand::LeaveEvent)
      {
      qvtkConnect(interactorStyle, event,
                  this, SLOT(processEvent(vtkObject*,void*,ulong,void*)));
      }
    d->ObservedInteractorStyles << interactorStyle;
    }
}

//------------------------------------------------------------------------------
void qMRMLTransformInfoWidget::processEvent(
  vtkObject* caller, void* callData, unsigned long eventId, void* clientData)
{
  Q_D(qMRMLTransformInfoWidget);
  Q_UNUSED(callData);
  Q_UNUSED(clientData);

  if (eventId == vtkCommand::LeaveEvent)
    {
    d->resetLabels();
    }
  else if (eventId == vtkCommand::EnterEvent || eventId == vtkCommand::MouseMoveEvent)
    {
    // Compute RAS
    vtkInteractorObserver * interactorStyle = vtkInteractorObserver::SafeDownCast(caller);
    Q_ASSERT(d->ObservedInteractorStyles.indexOf(interactorStyle) != -1);
    vtkRenderWindowInteractor * interactor = interactorStyle->GetInteractor();
    int xy[2] = {-1, -1};
    interactor->GetEventPosition(xy);
    qMRMLSliceWidget * sliceWidget = d->slicerWidget(interactorStyle);
    Q_ASSERT(sliceWidget);
    const qMRMLSliceView * sliceView = sliceWidget->sliceView();
    QList<double> xyz = const_cast<qMRMLSliceView *>(sliceView)->convertDeviceToXYZ(
      QList<int>() << xy[0] << xy[1]);
    QList<double> ras = const_cast<qMRMLSliceView *>(sliceView)->convertXYZToRAS(xyz);

    vtkMRMLSliceLogic * sliceLogic = sliceWidget->sliceLogic();
    vtkMRMLSliceNode * sliceNode = sliceWidget->mrmlSliceNode();

    // RAS
    if (d->MRMLTransformNode)
      {
      vtkAbstractTransform* Trans2Parent = d->MRMLTransformNode->GetTransformToParent();

      double* rasDisplaced = Trans2Parent->TransformDoublePoint(ras[0], ras[1], ras[2]);
      d->ViewerDisplacementVectorRAS->setText(QString("Displacement Vector in RAS: (%1, %2, %3)").
         arg(rasDisplaced[0] - ras[0], /* fieldWidth= */ 0, /* format = */ 'f', /* precision= */ 1).
         arg(rasDisplaced[1] - ras[1], /* fieldWidth= */ 0, /* format = */ 'f', /* precision= */ 1).
         arg(rasDisplaced[2] - ras[2], /* fieldWidth= */ 0, /* format = */ 'f', /* precision= */ 1));
      }
    }
}

//------------------------------------------------------------------------------
vtkMRMLTransformNode* qMRMLTransformInfoWidget::mrmlTransformNode()const
{
  Q_D(const qMRMLTransformInfoWidget);
  return d->MRMLTransformNode;
}

//------------------------------------------------------------------------------
void qMRMLTransformInfoWidget::setMRMLTransformNode(vtkMRMLNode* node)
{
  this->setMRMLTransformNode(vtkMRMLTransformNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qMRMLTransformInfoWidget::setMRMLTransformNode(vtkMRMLTransformNode* transformNode)
{
  Q_D(qMRMLTransformInfoWidget);
  qvtkReconnect(d->MRMLTransformNode, transformNode, vtkCommand::ModifiedEvent,
                this, SLOT(updateWidgetFromMRML()));
  d->MRMLTransformNode = transformNode;
  this->updateWidgetFromMRML();
}

//------------------------------------------------------------------------------
void qMRMLTransformInfoWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLTransformInfoWidget);
  if (d->MRMLTransformNode)
    {
    d->TransformToParentInfoTextBrowser->setText(d->MRMLTransformNode->GetTransformToParentInfo());
    d->TransformFromParentInfoTextBrowser->setText(d->MRMLTransformNode->GetTransformFromParentInfo());
    }
  else
    {
    d->TransformToParentInfoTextBrowser->setText("");
    d->TransformFromParentInfoTextBrowser->setText("");
    }

  this->setEnabled(d->MRMLTransformNode != 0);
}
