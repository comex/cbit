import subprocess, sys, re
cfile, exefile = sys.argv[1:]
try:
    output = subprocess.check_output([exefile])
except subprocess.CalledProcessError as e:
    output = e.output
    returncode = e.returncode
else:
    returncode = 0
output = output.strip()
text = open(cfile).read()

fail = False
got_something = False

for x in re.findall('^expect-exit ([0-9]+)', text, re.M):
    got_something = True
    x = int(x)
    if returncode != x:
        print 'got exit %d; expected %d' % (returncode, x)
        fail = True

for x in re.findall('^expect-output<<(.*)?>>', text, re.M | re.S):
    got_something = True
    x = x.strip()
    if output != x:
        print 'got output:\n%s\nexpected:\n%s' % (output, x)
        fail = True

assert got_something

if fail: sys.exit(1)
