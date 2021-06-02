#include "pch.h"

#if 0

class OcclusionFilterEffectXAudio2: public CXAPOBase, public IXAPOParameters 
{
public:
  OcclusionFilterEffectXAudio2(const XAPO_REGISTRATION_PROPERTIES *pRegProperties);
  virtual ~OcclusionFilterEffectXAudio2() {}
  
  // XAPO methods
  STDMETHOD_(void, Process) (UINT32 InputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS** ppInputProcessParameters, UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS** ppOutputProcessParameters);
  STDMETHOD(LockForProcess) (UINT32 InputLockedParameterCount, XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS** ppInputProcessParameters, UINT32 OutputLockedParameterCount, XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS** ppOutputProcessParameters);
  
  
  // Parameter Interface implementation.
  // Expects to handle a single floating point value (occlusionLevel).
  STDMETHOD_(void, GetParameters)  ( void *pParameters, UINT32 ParametersByteSize ) 
  {
    assert(ParametersByteSize == 4); 
    *(float*)pParameters = occlusionLevel;
  }
  
  STDMETHOD_(void, SetParameters)  (const void *pParameters, UINT32 ParametersByteSize ) 
  {
    assert(ParametersByteSize == 4);
    occlusionLevel = *((float*)pParameters);
  }

private:
  int referenceCount;
  int channels;
  int bytesPerSample;
  float occlusionLevel;
  float lastOutput[6];
};

OcclusionFilterEffectXAudio2::OcclusionFilterEffectXAudio2(const XAPO_REGISTRATION_PROPERTIES *pRegProperties)
:CXAPOBase(pRegProperties) 
{
  referenceCount = 0;
  occlusionLevel = 0.5f;
  channels = 1;
  bytesPerSample = 4;

  lastOutput[0] = 0.0f;
  lastOutput[1] = 0.0f;
  lastOutput[2] = 0.0f;
  lastOutput[3] = 0.0f;
  lastOutput[4] = 0.0f;
  lastOutput[5] = 0.0f;
}

STDMETHODIMP OcclusionFilterEffectXAudio2::LockForProcess(UINT32 InputLockedParameterCount, 
														  XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS** ppInputProcessParameters, 
														  UINT32 OutputLockedParameterCount, 
														  XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS** ppOutputProcessParameters) 
{
  channels			= ppInputProcessParameters[0]->pFormat->nChannels;
  bytesPerSample	= (ppInputProcessParameters[0]->pFormat->wBitsPerSample >> 3);
  
  return CXAPOBase::LockForProcess(
    InputLockedParameterCount, 
    ppInputProcessParameters, 
    OutputLockedParameterCount, 
    ppOutputProcessParameters);
}

STDMETHODIMP_(void) OcclusionFilterEffectXAudio2::Process(UINT32 InputProcessParameterCount, 
														  XAPO_PROCESS_BUFFER_PARAMETERS** ppInputProcessParameters, 
														  UINT32 OutputProcessParameterCount, 
														  XAPO_PROCESS_BUFFER_PARAMETERS** ppOutputProcessParameters) 
{
  
  void* pInputBuffer = ppInputProcessParameters[0]->pBuffer;
  assert(pInputBuffer != NULL);

  void* pOutputBuffer = ppOutputProcessParameters[0]->pBuffer;
  assert(pOutputBuffer != NULL);
    
  // one-pole low pass:
  float sampleRate = 48000.0f;
  float cutOffFreq = 500.0f;
  float lowFrequencyRatio = 0.25f; // ie. attenuate -2.5db at f=0

  if(occlusionLevel<0.01f) occlusionLevel = 0.01f;
  if(occlusionLevel>0.99f) occlusionLevel = 0.99f;

  float w = 2.0f * PI * cutOffFreq / sampleRate;
  float g = (1.0f-occlusionLevel);
  float cosw = Cos(w);
  float a = (1.0f - g*cosw - Sqrt(2.0f*g*(1.0f-cosw)-g*g*(1.0f-cosw*cosw))) / (1.0f-g);
  float k = g;

    for(int i=0; i<channels; i++)
	{
    for(int j=0; j<ppInputProcessParameters[0]->ValidFrameCount; j++) 
	{
      float sample	= ((float*)pInputBuffer)[i*ppInputProcessParameters[0]->ValidFrameCount + j];
      lastOutput[i] = (1.0f-a) * sample + a*lastOutput[i];
      ((float*)pOutputBuffer)[i*ppInputProcessParameters[0]->ValidFrameCount + j] = lastOutput[i];
    }
  }

  ppOutputProcessParameters[0]->ValidFrameCount = ppInputProcessParameters[0]->ValidFrameCount; 
  ppOutputProcessParameters[0]->BufferFlags     = ppInputProcessParameters[0]->BufferFlags;
}

#endif