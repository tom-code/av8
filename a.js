

var vvv = 0

function z()
{
  return 'abcd'
}

function b()
{
  vvv = vvv+1
  myfunc('bbb'+vvv)
  return 'aaaaaaaaaa'
}

function a(param)
{
  vvv = vvv+1
  myfunc('aaa'+vvv)
  param.mfo('xyz')
//  myfunc(param['anc'])
//  myfunc(param.path)
//  return param.path

  return "nazdar vole".replace(/vole/, "kole")
}


z()
