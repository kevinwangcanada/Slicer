/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kevin Wang, PMH.
  and was partially funded by OCAIRO and Sparkit.

==============================================================================*/

#ifndef __vtkMRMLTableStorageNode_h
#define __vtkMRMLTableStorageNode_h

#include "vtkMRMLStorageNode.h"

/// \brief MRML node for representing a volume storage
///
/// vtkMRMLTableStorageNode nodes describe the fiducial storage
/// node that allows to read/write point data from/to file.
class VTK_MRML_EXPORT vtkMRMLTableStorageNode : public vtkMRMLStorageNode
{
public:
  static vtkMRMLTableStorageNode *New();
  vtkTypeMacro(vtkMRMLTableStorageNode,vtkMRMLStorageNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName()  {return "TableStorage";};

  /// Return a default file extension for writting
  virtual const char* GetDefaultWriteFileExtension();

  /// Return true if the node can be read in
  virtual bool CanReadInReferenceNode(vtkMRMLNode *refNode);

protected:
  vtkMRMLTableStorageNode();
  ~vtkMRMLTableStorageNode();
  vtkMRMLTableStorageNode(const vtkMRMLTableStorageNode&);
  void operator=(const vtkMRMLTableStorageNode&);

  /// Initialize all the supported write file types
  virtual void InitializeSupportedReadFileTypes();

  /// Initialize all the supported write file types
  virtual void InitializeSupportedWriteFileTypes();

  /// Read data and set it in the referenced node
  virtual int ReadDataInternal(vtkMRMLNode *refNode);

  /// Write data from a  referenced node
  virtual int WriteDataInternal(vtkMRMLNode *refNode);

};

#endif


