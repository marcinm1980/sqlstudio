using System;

namespace MySqlStudio.X
{
  [AttributeUsage(AttributeTargets.Assembly)]
  class AssemblyReleaseTypeAttribute: Attribute
  {
    private readonly string _releaseType;
    public AssemblyReleaseTypeAttribute() : this(string.Empty) 
    { 
    }
    
    public AssemblyReleaseTypeAttribute(string value) 
    {
      _releaseType = value; 
    }
    
    public string ReleaseType
    {
      get { return _releaseType; }
    }
  }
}
