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


#ifndef __qMRMLTransformDisplayNodeWidgetPlugin_h
#define __qMRMLTransformDisplayNodeWidgetPlugin_h

#include "qSlicerTablesModuleWidgetsAbstractPlugin.h"

class Q_SLICER_MODULE_TABLES_WIDGETS_PLUGINS_EXPORT
qMRMLTableWidgetPlugin
  : public QObject, public qSlicerTablesModuleWidgetsAbstractPlugin
{
  Q_OBJECT

public:
  qMRMLTableWidgetPlugin(QObject *_parent = 0);

  QWidget *createWidget(QWidget *_parent);
  QString  domXml() const;
  QString  includeFile() const;
  bool     isContainer() const;
  QString  name() const;

};

#endif
