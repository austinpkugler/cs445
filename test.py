import os
import shutil
import sys


class Test:

    def __init__(self, dir, name):
        self._dir = dir
        self._name = name

    def __repr__(self):
        return f'Test(dir=\'{self._dir}\', name=\'{self._name}\')'

    def get_dir(self):
        return self._dir

    def get_name(self):
        return self._name

    def run(self, compiler, show_diff=True):
        passed = True

        src = os.path.join(self._dir, self._name + '.c-')
        tmp, actual, expected, diff = self.mktmp()

        out = os.path.join(self._dir, self._name + '.out')
        expected_out = os.path.join(expected, self._name + '.out')
        os.system(f'sort {out} > {expected_out}')

        tmp_out = os.path.join(tmp, 'tmp.out')
        os.system(f'{compiler} {src} > {tmp_out}')

        actual_out = os.path.join(actual, self._name + '.out')
        os.system(f'sort {tmp_out} > {actual_out}')

        if Test.diff(expected_out, actual_out):
            passed = False

            diff_out = os.path.join(diff, self._name + '.out')
            os.system(f'diff {expected_out} {actual_out} > {diff_out}')
            if show_diff:
                os.system(f'diff {expected_out} {actual_out}')

        os.remove(tmp_out)

        return passed

    def mktmp(self):
        tmp = os.path.join(self._dir, 'tmp')
        actual = os.path.join(tmp, 'actual')
        expected = os.path.join(tmp, 'expected')
        diff = os.path.join(tmp, 'diff')

        if not os.path.exists(tmp):
            os.mkdir(tmp)
            os.mkdir(actual)
            os.mkdir(expected)
            os.mkdir(diff)

        return tmp, actual, expected, diff

    def rmtmp(self):
        tmp = os.path.join(self._dir, 'tmp')
        if os.path.exists(tmp):
            shutil.rmtree(tmp)

    @classmethod
    def diff(cls, path1, path2):
        are_different = True

        file1 = open(path1, 'rb')
        file2 = open(path2, 'rb')
        if file1.read() == file2.read():
            are_different = False

        file1.close()
        file2.close()
        return are_different

    @classmethod
    def get_tests(cls, dir):
        return [Test(dir, f[:-4]) for f in os.listdir(dir) if f.endswith('.out')]


class Make:

    def __init__(self, dir):
        self._dir = dir

    def __repr__(self):
        return f'Make(dir=\'{self._dir}\')'

    def get_dir(self):
        return self._dir

    def make(self):
        self._execute('make')

    def clean(self):
        self._execute('make clean')

    def tar(self):
        self._execute('make tar')

    def _execute(self, cmd):
        cwd = os.getcwd()
        os.chdir(self._dir)
        os.system(cmd)
        os.chdir(cwd)


class Emit:
    HEADER = '\033[95m'
    SUCCESS = '\033[92m'
    WARN = '\033[93m'
    ERROR = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

    @classmethod
    def br(cls, length, endc='\n'):
        print('=' * length, end=endc)

    @classmethod
    def bold(cls, msg, endc='\n'):
        print(f'{Emit.BOLD}{msg}{Emit.ENDC}', end=endc)

    @classmethod
    def underline(cls, msg, endc='\n'):
        print(f'{Emit.UNDERLINE}{msg}{Emit.ENDC}', endc)

    @classmethod
    def success(cls, msg, endc='\n'):
        Emit.bold(f'{Emit.SUCCESS}{msg}{Emit.ENDC}', endc)

    @classmethod
    def warn(cls, msg, endc='\n'):
        Emit.bold(f'{Emit.WARN}{msg}{Emit.ENDC}', endc)

    @classmethod
    def error(cls, msg, endc='\n'):
        Emit.bold(f'{Emit.ERROR}{msg}{Emit.ENDC}', endc)


if __name__ == '__main__':
    argc = len(sys.argv)
    if argc < 3:
        print('Usage: src_dir test_dir -flag -flag')
        print('For this project:')
        print('$ python3 test.py hw1/ hw1/test')
        print('$ python3 test.py hw2/ hw2/test -p')
    elif argc == 3:
        compiler = os.path.join(sys.argv[1], 'c-')
    elif argc > 3:
        compiler = os.path.join(sys.argv[1], 'c-')
        flags = ' '.join(sys.argv[3:])

    make = Make(sys.argv[1])
    if os.path.exists(compiler):
        make.clean()

    make.make()
    if not os.path.exists(compiler):
        raise Exception('Compilation failed')

    passed = 0
    tests = Test.get_tests(sys.argv[2])
    for i, test in enumerate(tests):
        print(f'Running test {test} {i + 1}/{len(tests)}...', end='')
        if test.run(compiler + ' ' + flags):
            passed += 1
            Emit.success(f'[ PASSED ]')
        else:
            Emit.error(f'[ FAILED ]')

    if passed == len(tests):
        Emit.success('=' * 30)
        Emit.success(f'Passed {passed}/{len(tests)} tests')
        Emit.success('=' * 30)
    else:
        Emit.error('=' * 30)
        Emit.error(f'Passed {passed}/{len(tests)} tests')
        Emit.error('=' * 30)

    make.clean()
