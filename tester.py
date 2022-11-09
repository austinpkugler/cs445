import os
import shutil
import sys
import time


class Tester:

    def __init__(self, dir, sort=False, showdiff=False, notree=False):
        self.sort = sort
        self.showdiff = showdiff
        self.notree = notree
        self._src_dir = os.path.join(dir, 'src')
        self._test_dir = os.path.join(dir, 'test')
        self._tmp_dir = os.path.join(dir, 'tmp')

        if not os.path.exists(self._tmp_dir):
            os.mkdir(self._tmp_dir)

    def run_all(self, flags=''):
        tests = [f[:-3] for f in os.listdir(self._test_dir) if f.endswith('.c-')]

        passed = 0
        total_diff_count = 0
        for i, test in enumerate(tests):
            print(f'Running {test} {i + 1}/{len(tests)}...', end='')
            diff_count = self.run(test, flags=flags, clean=False)
            if not diff_count:
                passed += 1
                self.success_msg(f'[ PASSED (diff {diff_count}) ]')
            else:
                self.error_msg(f'[ FAILED (diff {diff_count}) ]')
            total_diff_count += diff_count

        if passed == len(tests):
            self.remove_tmp()
            self.success_msg('=' * 32)
            self.success_msg(f'PASSED {passed}/{len(tests)} (diff {total_diff_count})')
            self.success_msg('=' * 32)
        else:
            self.error_msg('=' * 32)
            self.error_msg(f'FALIED {passed}/{len(tests)} (diff {total_diff_count})')
            self.error_msg('=' * 32)

        self.execute(self._src_dir, 'make clean')
        return passed, tests, total_diff_count

    def run(self, test, flags='', clean=True):
        src = os.path.join(self._test_dir, test + '.c-')
        out = os.path.join(self._test_dir, test + '.out')
        actual = os.path.join(self._tmp_dir, test + '.actual')
        expected = os.path.join(self._tmp_dir, test + '.expected')
        compiler = os.path.join(self._src_dir, 'c-')

        if not os.path.exists(compiler):
            self.execute(self._src_dir, 'make')

        src_cp = os.path.join(self._tmp_dir, test + '.c-')
        os.system(f'cp {src} {src_cp}')

        if self.sort:
            os.system(f'sort {out} > {expected}')
            tmp_actual = os.path.join(self._tmp_dir, 'tmp.out')
            os.system(f'{compiler} {src} {flags} > {tmp_actual}')
            os.system(f'sort {tmp_actual} > {actual}')
            os.remove(tmp_actual)
        else:
            os.system(f'cp {out} {expected}')
            os.system(f'{compiler} {src} {flags} > {actual}')

        if self.notree:
            self.remove_tree(expected)
            self.remove_tree(actual)

        if clean:
            self.execute(self._src_dir, 'make clean')

        diff = os.path.join(self._tmp_dir, test + '.diff')
        os.system(f'diff {expected} {actual} > {diff}')

        diff_count = self.count_diff(diff)
        if not diff_count:
            os.remove(expected)
            os.remove(actual)
        else:
            if self.showdiff:
                os.system(f'diff {expected} {actual}')
        return diff_count

    def remove_tmp(self):
        if os.path.exists(self._tmp_dir):
            shutil.rmtree(self._tmp_dir)

    @staticmethod
    def count_diff(diff):
        diff_count = 0
        with open(diff, 'r') as file:
            for line in file.readlines():
                if not line.startswith('>') and not line.startswith('<'):
                    diff_count += 1
        return diff_count

    @staticmethod
    def execute(dir, cmd):
        cwd = os.getcwd()
        os.chdir(dir)
        os.system(cmd)
        os.chdir(cwd)

    @staticmethod
    def remove_tree(out):
        lines = []
        with open(out, 'r') as file:
            for line in file:
                if line.startswith('ERROR') or line.startswith('WARNING') or line.startswith('Number of '):
                    lines.append(line)
        with open(out, 'w') as file:
            file.write(''.join(lines))

    @staticmethod
    def bold_msg(msg, endc='\n'):
        print(f'\033[1m{msg}\033[0m', end=endc)

    @staticmethod
    def error_msg(msg, endc='\n'):
        Tester.bold_msg(f'\033[91m{msg}\033[0m', endc)

    @staticmethod
    def success_msg(msg, endc='\n'):
        Tester.bold_msg(f'\033[92m{msg}\033[0m', endc)

    @staticmethod
    def warn_msg(msg, endc='\n'):
        Tester.bold_msg(f'\033[93m{msg}\033[0m', endc)


def help():
    print('Usage: python3 tester.py hw_dir -flag --flag')

    print('Test Flags:')
    print('--help \t Displays this help menu.')
    print('--sort \t Sort the output files before diff.')
    print('--showdiff \t Shows test diffs in the terminal.')
    print('--notree \t Remove all AST output before diffing.')
    print('--rmtmp \t Remove the \'tmp/\' directory after testing.')

    print('Compiler Flags:')
    print('-d: \t Turn on parser debugging.')
    print('-D: \t Turn on symbol table debugging.')
    print('-h: \t Print compiler usage message')
    print('-p: \t Print the abstract syntax tree.')
    print('-P: \t Print the abstract syntax tree plus type information.')

    print('For this project:')
    print('$ python3 tester.py hw1/')
    print('$ python3 tester.py hw2/ -p')
    print('$ python3 tester.py hw3/ -p --sort')
    print('$ python3 tester.py hw4/ -P --sort')
    print('$ python3 tester.py hw5/ -P --sort')


if __name__ == '__main__':
    test_flags = {'--help': False, '--sort': False, '--showdiff': False, '--notree': False, '--rmtmp': False}
    compiler_flags = ''

    argc = len(sys.argv)
    if argc < 2:
        help()
        raise Exception('Insufficient args provided')
    elif argc == 2:
        if sys.argv[1] == '--help':
            help()
            sys.exit()
        if not os.path.exists(sys.argv[1]) or not os.path.isdir(sys.argv[1]):
            raise Exception('Invalid directory provided')
    elif argc > 2:
        cmd_flags = sys.argv[2:]
        for flag, value in test_flags.items():
            if flag in cmd_flags:
                test_flags[flag] = True
                cmd_flags.remove(flag)
        compiler_flags = ' '.join(cmd_flags)

    tester = Tester(sys.argv[1], sort=test_flags['--sort'], showdiff=test_flags['--showdiff'], notree=test_flags['--notree'])

    passed, tests, diff_count = tester.run_all(compiler_flags)
    with open('test.history', 'a') as history:
        cmd = ' '.join(sys.argv)
        history.write(f'{time.time()},"python3 {cmd}",{passed},{tests},{diff_count}\n')

    if test_flags['--rmtmp']:
        tester.remove_tmp()
