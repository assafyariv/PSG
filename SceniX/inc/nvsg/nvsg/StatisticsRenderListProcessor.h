// Copyright NVIDIA Corporation 2010
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#pragma once

#include <nvsg/IteratingRenderListProcessor.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace nvsg 
{
  
class StatisticsRenderListProcessor : public IteratingRenderListProcessor
{
public:
  NVSG_API StatisticsRenderListProcessor();
  NVSG_API virtual ~StatisticsRenderListProcessor();
  NVSG_API std::string getStatistics();

protected:
  virtual bool beginHandle( const RenderList2SharedPtr & ); 
  virtual bool beginHandle( const RenderModeGroupSharedPtr & ); 
  virtual bool beginHandle( const MaterialGroupSharedPtr & ); 
  
  virtual void endHandle( const RenderList2SharedPtr & ); 
  virtual void endHandle( const RenderModeGroupSharedPtr & ); 
  virtual void endHandle( const MaterialGroupSharedPtr & ); 

  virtual void handle( const DrawableInstanceSharedPtr & ); 

private:
  std::string makeName( const std::string & name, unsigned int number );
  std::string makeRow( const std::string & name, const std::string & value ); 
  std::string makeNode( const std::string & name, const std::string & data );
  std::string makeLink( const std::string & parent, const std::string & child ); 

  template <typename T> std::string toStr( T t );

private:
  typedef std::map< unsigned int, unsigned int > Histogram;

private:
  unsigned int m_numRMs;
  unsigned int m_numMGs;
  unsigned int m_numDIs;
  unsigned int m_numLIs;

  Histogram m_RLHist;
  Histogram m_RMHist;
  Histogram m_MGHist;

  std::vector<std::string> parentVector;
  std::stringstream m_ss;
};


} // namespace nvsg



