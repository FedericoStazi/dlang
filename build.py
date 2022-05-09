#!/usr/bin/python3

# Copyright 2022 Federico Stazi. Subject to the MIT license.

"""This module provides a command line interface to build projects"""

import os
import shutil
import subprocess
import sys
from termcolor import colored

class CMake():
  """Class managing the build process of CMake projects"""

  def __init__(self, target, path = "", *, copy=True):
    """Construct the object for a certain target"""
    self.target = target
    self.path = path + target
    self.copy = copy

  def clean(self):
    """Clean anything created by previous builds"""
    try:
      if "build" in os.listdir(self.path):
        shutil.rmtree(f"{self.path}/build/")
      if self.copy and self.target in os.listdir(self.path):
        os.remove(f"{self.path}/{self.target}")
      print(colored(f"Clean {self.target} succeeded", "green"))
    except Exception as ex:
      print(colored(f"Clean {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))

  def build(self):
    """Run the build process"""
    print(f"Building {self.target}...", end = "\r")
    try:
      origin = os.getcwd()
      os.chdir(self.path)
      if "build" not in os.listdir():
        os.mkdir("build")
      os.chdir("build")
      subprocess.check_output(["cmake", ".."],
                              stderr=subprocess.STDOUT)
      subprocess.check_output(["cmake", "--build", "."],
                              stderr=subprocess.STDOUT)
      if self.copy:
        shutil.copyfile(self.target, f"../{self.target}")
        shutil.copystat(self.target, f"../{self.target}")
      print(colored(f"Build {self.target} succeeded", "green"))
    except OSError as ex:
      print(colored(f"Build {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))
    except subprocess.CalledProcessError as ex:
      print(colored(f"Build {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))
      print(str(ex.stdout, "utf8"))
    finally:
      os.chdir(origin)

class Ocamlbuild():
  """Class managing the build process of Ocamlbuild projects"""

  def __init__(self, target, path = "", *, copy=True):
    """Construct the object for a certain target"""
    self.target = target
    self.path = path + target
    self.copy = copy

  def clean(self):
    """Clean anything created by previous builds"""
    try:
      origin = os.getcwd()
      os.chdir(self.path)
      subprocess.check_output(["ocamlbuild", "-clean"],
                              stderr=subprocess.STDOUT)
      if self.copy and self.target in os.listdir("."):
        os.remove(f"{self.target}")
      if self.copy and f"{self.target}.byte" in os.listdir("."):
        os.remove(f"{self.target}.byte")
      print(colored(f"Clean {self.target} succeeded", "green"))
    except Exception as ex:
      print(colored(f"Clean {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))
    finally:
      os.chdir(origin)

  def build(self):
    """Run the build process"""
    print(f"Building {self.target}...", end = "\r")
    try:
      origin = os.getcwd()
      os.chdir(self.path)
      dirs = ",".join(dir[2:] for dir, _, _ in os.walk(".")
                              if dir != "." and "_build" not in dir)
      subprocess.check_output(["ocamlbuild",
                                 "-Is", dirs,
                                 "-r", self.target + ".byte"],
                              stderr=subprocess.STDOUT)
      if self.copy:
        shutil.copyfile(self.target + ".byte", self.target)
        shutil.copystat(self.target + ".byte", self.target)
      print(colored(f"Build {self.target} succeeded", "green"))
    except OSError as ex:
      print(colored(f"Build {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))
    except subprocess.CalledProcessError as ex:
      print(colored(f"Build {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))
      print(str(ex.stdout, "utf8"))
    finally:
      os.chdir(origin)

class RPython():
  """Class managing the compilation process of RPython projects"""

  def __init__(self, target, path = "./", *, copy=True):
    """Construct the object for a certain target"""
    self.target = target
    self.path = path
    self.copy = copy

  def clean(self):
    """Clean anything created by previous builds"""
    try:
      if f"{self.target}-c" in os.listdir(self.path):
        os.remove(f"{self.path}/{self.target}-c")
      if f"{self.target}.pyc" in os.listdir(self.path):
        os.remove(f"{self.path}/{self.target}.pyc")
      if self.copy and self.target in os.listdir(self.path):
        os.remove(self.target)
      print(colored(f"Clean {self.target} succeeded", "green"))
    except Exception as ex:
      print(colored(f"Clean {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))

  def build(self):
    """Run the build process"""
    print(f"Building {self.target}...", end = "\r")
    try:
      origin = os.getcwd()
      if self.path:
        os.chdir(self.path)
      subprocess.check_output(["rpython", "--opt=jit", self.target + ".py"],
                              stderr=subprocess.STDOUT)
      if f"{self.target}.pyc" in os.listdir("."):
        os.remove(f"{self.target}.pyc")
      if self.copy:
        shutil.copyfile(self.target + "-c", self.target)
        shutil.copystat(self.target + "-c", self.target)
      print(colored(f"Build {self.target} succeeded", "green"))
    except OSError as ex:
      print(colored(f"Build {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))
    except subprocess.CalledProcessError as ex:
      print(colored(f"Build {self.target} failed, " +
                    f"{type(ex).__name__}: {ex}", "red"))
      print(str(ex.stdout, "utf8"))
    finally:
      os.chdir(origin)


if __name__ == "__main__":
  options = {
    "clean"    : lambda: (Ocamlbuild("dlang_c").clean(),
                          Ocamlbuild("slang", "cc_cl_cam_ac_uk/",
                                     copy=False).clean(),
                          CMake("dlang_vm").clean(),
                          RPython("meta_dlang_vm").clean()),
    "dlang-c"  : lambda: (Ocamlbuild("dlang_c").build()),
    "slang"    : lambda: (Ocamlbuild("slang", "cc_cl_cam_ac_uk/",
                                     copy=False).build()),
    "dlang-vm" : lambda: (CMake("dlang_vm").build()),
    "meta"     : lambda: (RPython("meta_dlang_vm").build()),
    "all"      : lambda: (Ocamlbuild("dlang_c").build(),
                          Ocamlbuild("slang", "cc_cl_cam_ac_uk/",
                                     copy=False).build(),
                          CMake("dlang_vm").build())
  }

  file_path = os.path.dirname(os.path.realpath(__file__))
  os.chdir(file_path)

  if len(sys.argv) < 2 or sys.argv[1] not in options:
    print ("Usage:" + "\n\t./build.py ".join([""] + list(options)))
  else:
    options[sys.argv[1]]()
