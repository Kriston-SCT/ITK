/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkSyNImageRegistrationMethod_h
#define __itkSyNImageRegistrationMethod_h

#include "itkImageRegistrationMethodv4.h"

#include "itkDisplacementFieldTransform.h"

namespace itk
{
//Forward-declare these because of module dependency conflict.
//They will soon be moved to a different module, at which
// time this can be removed.
template <unsigned int VDimension, class TDataHolder>
class ImageToData;
template <class TDataHolder>
class Array1DToData;

/** \class SyNImageRegistrationMethod
 * \brief Interface method for the performing greedy SyN image registration.
 *
 * For greedy SyN we use \c m_Transform to map the time-parameterized middle
 * image to the fixed image (and vice versa using
 * \c m_Transform->GetInverseDisplacementField() ).  We employ another ivar,
 * \c m_InverseTransform, to map the time-parameterized middle image to the
 * moving image.
 *
 * Output: The output is the updated transform which has been added to the
 * composite transform.
 *
 *
 * \author Nick Tustison
 * \author Brian Avants
 *
 * \ingroup ITKRegistrationMethodsv4
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform =
  DisplacementFieldTransform<double, GetImageDimension<TFixedImage>::ImageDimension> >
class ITK_EXPORT SyNImageRegistrationMethod
: public ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
{
public:
  /** Standard class typedefs. */
  typedef SyNImageRegistrationMethod                                            Self;
  typedef ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>      Superclass;
  typedef SmartPointer<Self>                                                    Pointer;
  typedef SmartPointer<const Self>                                              ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** ImageDimension constants */
  itkStaticConstMacro( ImageDimension, unsigned int, TFixedImage::ImageDimension );

  /** Run-time type information (and related methods). */
  itkTypeMacro( SyNImageRegistrationMethod, SimpleImageRegistrationMethod );

  /** Input typedefs for the images and transforms. */
  typedef TFixedImage                                                 FixedImageType;
  typedef typename FixedImageType::Pointer                            FixedImagePointer;
  typedef TMovingImage                                                MovingImageType;
  typedef typename MovingImageType::Pointer                           MovingImagePointer;

  /** Metric and transform typedefs */
  typedef typename Superclass::MetricType                             MetricType;
  typedef typename MetricType::Pointer                                MetricPointer;
  typedef typename MetricType::VirtualImageType                       VirtualImageType;

  typedef TTransform                                                  TransformType;
  typedef typename TransformType::Pointer                             TransformPointer;
  typedef typename TransformType::ScalarType                          RealType;
  typedef typename TransformType::DerivativeType                      DerivativeType;
  typedef typename DerivativeType::ValueType                          DerivativeValueType;
  typedef typename TransformType::DisplacementFieldType               DisplacementFieldType;
  typedef typename DisplacementFieldType::Pointer                     DisplacementFieldPointer;
  typedef typename DisplacementFieldType::PixelType                   DisplacementVectorType;

  typedef CompositeTransform<RealType, ImageDimension>                CompositeTransformType;
  typedef typename CompositeTransformType::TransformType              TransformBaseType;

  typedef typename Superclass::TransformOutputType                    TransformOutputType;
  typedef typename TransformOutputType::Pointer                       TransformOutputPointer;

  typedef Array<SizeValueType>                                        NumberOfIterationsArrayType;

  /** Set/Get the learning rate. */
  itkSetMacro( LearningRate, RealType );
  itkGetConstMacro( LearningRate, RealType );

  /** Set/Get the number of iterations per level. */
  itkSetMacro( NumberOfIterationsPerLevel, NumberOfIterationsArrayType );
  itkGetConstMacro( NumberOfIterationsPerLevel, NumberOfIterationsArrayType );

  /** Set/Get the convergence threshold */
  itkSetMacro( ConvergenceThreshold, RealType );
  itkGetConstMacro( ConvergenceThreshold, RealType );

  /**
   * Get/Set the Gaussian smoothing standard deviation for the update field.
   * Default = 1.75.
   */
  itkSetMacro( GaussianSmoothingVarianceForTheUpdateField, RealType );
  itkGetConstReferenceMacro( GaussianSmoothingVarianceForTheUpdateField, RealType );

  /**
   * Get/Set the Gaussian smoothing standard deviation for the total field.
   * Default = 0.5.
   */
  itkSetMacro( GaussianSmoothingVarianceForTheTotalField, RealType );
  itkGetConstReferenceMacro( GaussianSmoothingVarianceForTheTotalField, RealType );

protected:
  SyNImageRegistrationMethod();
  virtual ~SyNImageRegistrationMethod();
  virtual void PrintSelf( std::ostream & os, Indent indent ) const;

  /** Perform the registration. */
  virtual void  GenerateData();

  /** Handle optimization internally */
  virtual void StartOptimization();

  /**
   * Initialize by setting the interconnects between the components. Need to override
   * in the SyN class since we need to "adapt" the \c m_InverseTransform
   */
  virtual void InitializeRegistrationAtEachLevel( const SizeValueType );

  virtual DisplacementFieldPointer ComputeUpdateField( const TFixedImage *, const TransformBaseType *, const TMovingImage *, const TransformBaseType * );
  virtual DisplacementFieldPointer GaussianSmoothDisplacementField( const DisplacementFieldType *, const RealType );

private:
  SyNImageRegistrationMethod( const Self & );   //purposely not implemented
  void operator=( const Self & );               //purposely not implemented

  RealType                                                        m_LearningRate;

  RealType                                                        m_GaussianSmoothingVarianceForTheUpdateField;
  RealType                                                        m_GaussianSmoothingVarianceForTheTotalField;

  TransformPointer                                                m_MiddleToMovingTransform;
  TransformPointer                                                m_MiddleToFixedTransform;

  RealType                                                        m_ConvergenceThreshold;

  NumberOfIterationsArrayType                                     m_NumberOfIterationsPerLevel;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSyNImageRegistrationMethod.hxx"
#endif

#endif