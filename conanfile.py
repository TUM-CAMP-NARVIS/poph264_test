from conan import ConanFile
from conan.tools.system.package_manager import Apt
from conan.tools.files import apply_conandata_patches, collect_libs, copy, export_conandata_patches, get, rename, replace_in_file, rmdir, save

class PopH264TestConan(ConanFile):
    python_requires = "camp_common/0.5@camposs/stable"
    python_requires_extend = "camp_common.CampCMakeBase"

    name = "poph264-test"
    version = "0.1.0"

    description = "PopH264 Test Application"
    url = "https://github.com/TUM-CAMP-NARVIS/poph264_test"
    license = "Internal"

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
    }

    default_options = {
        "shared": True,
        "opencv/*:with_eigen": True,
        "opencv/*:contrib": True,
        "opencv/*:neon": True,
        "opencv/*:with_jpeg": "libjpeg-turbo",

        "opencv/*:contrib_freetype": False,
        "opencv/*:contrib_sfm": False,
        "opencv/*:with_ade": False,
        "opencv/*:with_png": False,
        "opencv/*:with_tiff": False,
        "opencv/*:with_jpeg2000": False,
        "opencv/*:with_openexr": False,
        "opencv/*:with_webp": False,
        "opencv/*:with_gtk": False,
        "opencv/*:with_quirc": False,
        "opencv/*:with_cufft": False,
        "opencv/*:with_v4l": False,
        "opencv/*:with_ffmpeg": False,
        "opencv/*:with_imgcodec_hdr": False,
        "opencv/*:with_imgcodec_pfm": False,
        "opencv/*:with_imgcodec_pxm": False,
        "opencv/*:with_imgcodec_sunraster": False,
        "opencv/*:with_msmf": False,
        "opencv/*:with_msmf_dxva": False,
        "opencv/*:with_opengl": False,
        # "opencv/*:cuda_arch_bin": None,
        "opencv/*:cpu_baseline": None,
        "opencv/*:cpu_dispatch": None,
        "opencv/*:nonfree": False,    
    }

    # all sources are deployed with the package
    exports_sources = "cmake/*", "src/*", "CMakeLists.txt"

    @property
    def is_platform_win(self):
        return self.settings.os == "Windows" or self.settings.os == "WindowsStore"


    def requirements(self):
        self.requires("spdlog/1.11.0")
        self.requires("rapidjson/cci.20220822")
        self.requires("opencv/4.8.0@camposs/stable", override=True)
        self.requires("poph264/1.9.2@camposs/stable")

    def configure(self):
        self.options["opencv"].dnn_cuda = False
        self.options["opencv"].with_cuda = False
        self.options["opencv"].with_dnn = False
        self.options["opencv"].with_cublas = False
        self.options["opencv"].dnn = False
        self.options["opencv"].with_ipp = False
