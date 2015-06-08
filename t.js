



function test()
{
  log('nazdar '+msisdn);
  var msisdn = 'a';
  register_event('map_sri', function(m) {
     log('inside ' + msisdn);
     msisdn = 'abcd';
   }
  );


//  send_mo();

  log(msisdn);
  return 'aaa';
}

'abc'
