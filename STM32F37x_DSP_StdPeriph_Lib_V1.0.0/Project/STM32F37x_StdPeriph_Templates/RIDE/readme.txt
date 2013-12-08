/**
  @page ride RIDE Project Template for STM32F37x devices
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    readme.txt
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012 
  * @brief   This sub-directory contains all the user-modifiable files 
  *          needed to create a new project linked with the STM32F37x  
  *          Standard Peripheral Library and working with RIDE7 software 
  *          toolchain
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
   @endverbatim
 
 @par Directory contents
 
 - project .rprj/.rapp: A pre-configured project file with the provided library 
                        structure that produces an executable image with RIDE7.

 @par How to use it ?
 
 - Open the Project.rprj project.
 - In the configuration toolbar(Project->properties) select the project config:
     - STM32373C-EVAL: to configure the project for STM32F37x Ultra Low Power 
                       Medium-Density Devices
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STDPERIPH_DRIVER, STM32F37X, USE_STM32373C_EVAL
 - Rebuild all files: Project->build project
 - Load project image: Debug->start(ctrl+D)
 - Run program: Debug->Run(ctrl+F9)  

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


