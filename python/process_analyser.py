# python process_analyser.py -b firefox kdelop -i 1

#import curses
import psutil
import atexit
import time
import zmq
import json
import argparse
from datetime import datetime, timedelta
from optparse import OptionParser

#    """
#    >>> bytes2human(10000)
#    '9K'
#    >>> bytes2human(100001221)
#    '95M'
#    """

def bytes2human(n):
    symbols = ('K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y')
    prefix = {}
    for i, s in enumerate(symbols):
      prefix[s] = 1 << (i + 1) * 10
#      print "i:", i, ", prefix[",s,"]: ",prefix[s]
    for s in reversed(symbols):
#      print "n:", n, ", s:", s, ", prefix[",s,"]: ",prefix[s]
      if n >= prefix[s]:
        value = int(float(n) / prefix[s])
        return '%s%s' % (value, s)
    return "%sB" % n


def poll(interval, process_name):
    procs = []
    procs_status = {}
    for p in psutil.process_iter():
      try:
        p.dict = p.as_dict(['pid', 'name','username', 'nice', 'memory_info',
                            'memory_percent', 'cpu_percent',
                            'cpu_times', 'name', 'status'])
        try:
            if p.dict['name'] == process_name:
              procs.append(p)
            procs_status[p.dict['status']] += 1
        except KeyError:
            procs_status[p.dict['status']] = 1
      except psutil.NoSuchProcess:
          pass

    # return processes sorted by CPU percent usage
    processes = sorted(procs, key=lambda p: p.dict['cpu_percent'],
                       reverse=True)
#    print "*** ",type(processes),"processes:", processes
    return (processes, procs_status)

def get_process_data_dict(name=''):
  return {'pid':-1, 'username': '', 'status': '', 'cpu_times': -1, \
          'name': name, 'cpu_percent': -1.0, 'memory_percent': -1.0, \
          'memory_rss_info': 0, 'memory_rss_label': '',
          'memory_vms_info': 0, 'memory_vms_label': -1, 'nice': -1}

def get_populated_data_dict(p):
  data = get_process_data_dict(p.dict['name'])
  templ = "[%s %s %s %s %s %s %s %s %s]"
  if p.dict['cpu_times'] is not None:
#      ctime = timedelta(seconds=sum(p.dict['cpu_times']))
      data['cpu_times'] = str(timedelta(seconds=sum(p.dict['cpu_times'])))
  else:
      data['cpu_times'] = ''
#      ctime = ''
  if p.dict['memory_percent'] is not None:
      data['memory_percent'] = round(p.dict['memory_percent'], 1)
  else:
      data['memory_percent'] = ''
  if p.dict['cpu_percent'] is None:
      data['cpu_percent'] =''
  else:
      data['cpu_percent'] = p.dict['cpu_percent']
  if p.dict['username']:
      data['username'] = p.dict['username']
#      username = p.dict['username'][:8]
  else:
      data['username'] =''
#      username = ""
  data['pid'] = p.dict['pid']
  data['status'] = p.dict['status']
  data['name'] = p.dict['name']
  data['nice'] = p.dict['nice']
  data['memory_rss_info'] = getattr(p.dict['memory_info'], 'vms', 0)
  data['memory_vms_info'] = getattr(p.dict['memory_info'], 'rss', 0)
  data['memory_rss_label'] = bytes2human(data['memory_rss_info'])
  data['memory_vms_label'] = bytes2human(data['memory_vms_info'])
  return data

def main(options):
  print options
  # Prepare our context and publisher
  ctx = zmq.Context.instance()
  publisher = ctx.socket(zmq.PUB)
  publisher.bind("tcp://*:5563")
  topic = ""
  messagedata = {'data': []}

  try:
      interval = 0
      while 1:
        for processName in options.process:
          (processes, procs_status) = poll(interval, processName)
          if len(processes) > 0:
            for p in processes:
              messagedata['data'].append( get_populated_data_dict(p) )
          else:
            messagedata['data'].append( get_process_data_dict(processName) )

        # Sending messages
        msg = "%s %s" % ("process_info", json.dumps(messagedata))
        print "=>", msg
        publisher.send(msg)
        messagedata['data'] = []

        # Waiting interval
        time.sleep(interval)
        print
        interval = options.interval
  except (KeyboardInterrupt, SystemExit):
      pass

if __name__ == '__main__':
  parser2 = argparse.ArgumentParser(description='Process benchmarks.')
  parser2.add_argument("-b", "--process", default=[], type=str, nargs='+', help="process name")
  parser2.add_argument("-i", "--interval", dest="interval", help="delay interval", type=float)
#  parser2.add_argument("-p", "--process", dest="process", help="process name")
#  parser2.add_argument("-b", "--benchmark", default=[], type=str, nargs='+', help="The benchmark to be loaded.")
  args = parser2.parse_args()

  if args.process is None:
    parser.error('Process name not given')
    exit(0)

  main(args)
