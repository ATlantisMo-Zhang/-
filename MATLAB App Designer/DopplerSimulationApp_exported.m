classdef DopplerSimulationApp_exported < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure                     matlab.ui.Figure
        TextArea2                    matlab.ui.control.TextArea
        PlotsPanel                   matlab.ui.container.Panel
        UIAxes_TimeDomain            matlab.ui.control.UIAxes
        UIAxes_FrequencyDomain       matlab.ui.control.UIAxes
        StatusPanel                  matlab.ui.container.Panel
        StatusTextArea               matlab.ui.control.TextArea
        ControlsPanel                matlab.ui.container.Panel
        ClearParametersButton        matlab.ui.control.Button
        LoadResultsAndPlotButton     matlab.ui.control.Button
        GenerateConfigAndRunCButton  matlab.ui.control.Button
        ParametersPanel              matlab.ui.container.Panel
        SNRdBEditField               matlab.ui.control.NumericEditField
        dBEditFieldLabel             matlab.ui.control.Label
        Target1InitialAmplitudeEditField  matlab.ui.control.NumericEditField
        Label_5                      matlab.ui.control.Label
        NoiseEnabledCheckBox         matlab.ui.control.CheckBox
        Target2InitialAmplitudeEditField  matlab.ui.control.NumericEditField
        Label_4                      matlab.ui.control.Label
        Target2VelocityMpsEditField  matlab.ui.control.NumericEditField
        msEditFieldLabel_3           matlab.ui.control.Label
        Target1VelocityMpsEditField  matlab.ui.control.NumericEditField
        msEditFieldLabel_2           matlab.ui.control.Label
        NumTargetsDropDown           matlab.ui.control.DropDown
        Label_3                      matlab.ui.control.Label
        SoundSpeedMpsEditField       matlab.ui.control.NumericEditField
        msEditFieldLabel             matlab.ui.control.Label
        LFMBandwidthHzEditField      matlab.ui.control.NumericEditField
        LFMHzEditFieldLabel          matlab.ui.control.Label
        SamplingRateHzEditField      matlab.ui.control.NumericEditField
        HzEditFieldLabel             matlab.ui.control.Label
        PulseDurationSEditField      matlab.ui.control.NumericEditField
        sLabel_2                     matlab.ui.control.Label
        FrequencyHzEditField         matlab.ui.control.NumericEditField
        HzLabel_2                    matlab.ui.control.Label
        SignalTypeDropDown           matlab.ui.control.DropDown
        Label_2                      matlab.ui.control.Label
    end


    methods (Access = private)
        function [fd1, fd2, theory_fd1, theory_fd2] = analyze_frequency_domain(app, f, P1_original, P1_received)
            % 找到原始信号的主频
            [~, idx_original] = max(P1_original);
            f0 = f(idx_original); % 原始信号的主频

            % 获取目标数量和声速
            numTargets = str2double(app.NumTargetsDropDown.Value);
            c = app.SoundSpeedMpsEditField.Value;
            v1 = app.Target1VelocityMpsEditField.Value;

            % 计算目标1的理论频移量 (使用统一后的公式)
            theory_fd1 = f0 * (-v1 / (c + v1));
            fd1 = NaN; % 初始化为 NaN

            % 初始化目标2的参数
            theory_fd2 = 0;
            fd2 = NaN; % 初始化为 NaN
            v2 = 0; % 初始化v2

            if numTargets == 2
                v2 = app.Target2VelocityMpsEditField.Value;
                theory_fd2 = f0 * (-v2 / (c + v2)); % 使用统一后的公式
            end

            % 使用 findpeaks 寻找接收信号频谱中的峰值
            % 我们寻找最多 numTargets 个峰值，并要求峰值至少是最大峰值的一定比例，以滤除噪声
            % 'MinPeakProminence' 和 'MinPeakHeight' 可以根据实际信号情况调整
            [peaks_amp, peak_locs_indices, ~, peak_prominences] = findpeaks(P1_received, f, 'SortStr','descend', 'NPeaks', numTargets, 'MinPeakProminence', max(P1_received)*0.05);

            if isempty(peaks_amp)
                app.StatusTextArea.Value = [app.StatusTextArea.Value; '警告: 在接收信号频谱中未找到显著峰值。'];
                return;
            end

            % 提取实际频移
            % 第一个峰值总是认为是目标1 (如果存在)
            if ~isempty(peak_locs_indices)
                actual_f1 = peak_locs_indices(1);
                fd1 = actual_f1 - f0;
            end

            % 如果有两个目标且找到了第二个峰值
            if numTargets == 2 && length(peak_locs_indices) > 1
                actual_f2 = peak_locs_indices(2);
                fd2 = actual_f2 - f0;
                % 可选：根据理论频移尝试更智能地匹配峰值。
                % 例如，如果理论频移差异很大，可以检查哪个峰值更接近各自的理论值。
                % 为简化，这里我们假设最强的两个峰值对应目标1和目标2。
            elseif numTargets == 2 && length(peak_locs_indices) <= 1
                app.StatusTextArea.Value = [app.StatusTextArea.Value; '警告: 未能从频谱中明确分离目标2的信号。仅识别到一个主峰。'];
            end
        end



        % % 时域信号分析 - 修复版
        % % 时域信号分析 - 修复版 (采用findpeaks)
        function [delay1, delay2, theory_delay1, theory_delay2] = analyze_time_domain(app, ~, signalOriginal, ~, signalReceived)
            % 获取目标数量和声速
            numTargets = str2double(app.NumTargetsDropDown.Value);
            c = app.SoundSpeedMpsEditField.Value;

            % 理论时间延迟 (假设方案B，即已在C代码和UI中引入距离R)
            % 如果未实现方案B，则这部分理论值意义不大或应设为0
            R1 = 0; % 假设默认值，如果未从UI获取
            % 示例: R1 = app.Target1RangeMEditField.Value; % 如果有此控件
            theory_delay1 = (2 * R1) / c; % 假设R1是目标1的距离
            if isnan(theory_delay1) || isinf(theory_delay1) || R1 <=0 % 基本的有效性检查
                theory_delay1 = 0; % 如果距离无效或未设置，理论延迟为0
            end

            delay1 = NaN; % 初始化为 NaN

            theory_delay2 = 0;
            delay2 = NaN; % 初始化为 NaN
            R2 = 0; % 假设默认值

            if numTargets == 2
                % 示例: R2 = app.Target2RangeMEditField.Value;
                theory_delay2 = (2 * R2) / c; % 假设R2是目标2的距离
                if isnan(theory_delay2) || isinf(theory_delay2) || R2 <=0
                    theory_delay2 = 0;
                end
            end

            % 使用互相关计算时间延迟
            if isempty(signalReceived) || isempty(signalOriginal)
                app.StatusTextArea.Value = [app.StatusTextArea.Value; '警告: 时域分析的输入信号为空。'];
                return;
            end
            if length(signalReceived) ~= length(signalOriginal)
                % 如果长度不一致，可能需要截取或填充，或者直接报错
                % 为简单起见，这里先假设C程序保证输出长度一致
                % 若不一致，xcorr可能会出问题或结果不符合预期
            end

            [xcorr_result, lags] = xcorr(signalReceived, signalOriginal);

            % 确保 xcorr_result 不为空
            if isempty(xcorr_result)
                app.StatusTextArea.Value = [app.StatusTextArea.Value; '警告: 互相关结果为空。'];
                return;
            end

            % 使用 findpeaks 寻找互相关结果中的峰值
            % 我们寻找最多 numTargets 个峰值
            [peak_corr_values, peak_lag_indices_in_lags] = findpeaks(abs(xcorr_result), 'SortStr', 'descend', 'NPeaks', numTargets, 'MinPeakHeight', 0.1*max(abs(xcorr_result)));

            if isempty(peak_corr_values)
                app.StatusTextArea.Value = [app.StatusTextArea.Value; '警告: 在互相关结果中未找到显著峰值。'];
                return;
            end

            sample_delays_at_peaks = lags(peak_lag_indices_in_lags);

            % 提取实际时间延迟
            if ~isempty(sample_delays_at_peaks)
                delay1 = sample_delays_at_peaks(1) / app.SamplingRateHzEditField.Value;
            end

            if numTargets == 2 && length(sample_delays_at_peaks) > 1
                delay2 = sample_delays_at_peaks(2) / app.SamplingRateHzEditField.Value;
                % 可选：根据理论时延（如果方案B已完全实施并有可靠理论值）或峰值强度/顺序进行智能匹配
            elseif numTargets == 2 && length(sample_delays_at_peaks) <= 1
                app.StatusTextArea.Value = [app.StatusTextArea.Value; '警告: 未能从互相关结果中明确分离目标2的信号。仅识别到一个主延迟。'];
            end
        end




        % 综合对比分析函数 - 修复版
                % 综合对比分析函数 - 修复版 (处理NaN)
        function generate_analysis_result(app, fd1, fd2, theory_fd1, theory_fd2, delay1, delay2, theory_delay1, theory_delay2)
            % 频域信号分析结果
            freq_analysis = sprintf('频域信号分析：\n');
            if ~isnan(fd1)
                freq_analysis = [freq_analysis, sprintf('目标1：实际频移量 = %.2f Hz，理论频移量 = %.2f Hz\n', fd1, theory_fd1)];
            else
                freq_analysis = [freq_analysis, sprintf('目标1：未能检测到实际频移，理论频移量 = %.2f Hz\n', theory_fd1)];
            end

            if strcmp(app.NumTargetsDropDown.Value, '2') % 如果设置为双目标
                if ~isnan(fd2)
                    freq_analysis = [freq_analysis, sprintf('目标2：实际频移量 = %.2f Hz，理论频移量 = %.2f Hz\n', fd2, theory_fd2)];
                else
                    freq_analysis = [freq_analysis, sprintf('目标2：未能检测到实际频移，理论频移量 = %.2f Hz\n', theory_fd2)];
                end
            end

            % 时域信号分析结果
            time_analysis = sprintf('时域信号分析：\n');
             if ~isnan(delay1)
                time_analysis = [time_analysis, sprintf('目标1：实际时间延迟 = %.4f s，理论时间延迟 = %.4f s\n', delay1, theory_delay1)];
            else
                time_analysis = [time_analysis, sprintf('目标1：未能检测到实际时间延迟，理论时间延迟 = %.4f s\n', theory_delay1)];
            end

            if strcmp(app.NumTargetsDropDown.Value, '2') % 如果设置为双目标
                if ~isnan(delay2)
                    time_analysis = [time_analysis, sprintf('目标2：实际时间延迟 = %.4f s，理论时间延迟 = %.4f s\n', delay2, theory_delay2)];
                else
                    time_analysis = [time_analysis, sprintf('目标2：未能检测到实际时间延迟，理论时间延迟 = %.4f s\n', theory_delay2)];
                end
            end

            % 综合对比分析
            comparison = sprintf('综合对比：\n');
            v1 = app.Target1VelocityMpsEditField.Value;
            v2 = 0;
            numTargetsStr = app.NumTargetsDropDown.Value;

            if strcmp(numTargetsStr, '2')
                v2 = app.Target2VelocityMpsEditField.Value;
                % 仅当两个目标的速度和检测到的频移都有效时，进行详细对比
                if ~isnan(fd1) && ~isnan(fd2)
                    if abs(v2) > abs(v1)
                        comparison = [comparison, sprintf('目标2的速度(%.2f m/s)较目标1(%.2f m/s)更快。\n', v2, v1)];
                        if abs(fd2) > abs(fd1)
                            comparison = [comparison, sprintf('  其观察到的实际频移量(%.2f Hz)也大于目标1(%.2f Hz)。\n', abs(fd2), abs(fd1))];
                        else
                             comparison = [comparison, sprintf('  但其观察到的实际频移量(%.2f Hz)不一定大于目标1(%.2f Hz)。\n', abs(fd2), abs(fd1))];
                        end
                    elseif abs(v1) > abs(v2)
                        comparison = [comparison, sprintf('目标1的速度(%.2f m/s)较目标2(%.2f m/s)更快。\n', v1, v2)];
                        if abs(fd1) > abs(fd2)
                            comparison = [comparison, sprintf('  其观察到的实际频移量(%.2f Hz)也大于目标2(%.2f Hz)。\n', abs(fd1), abs(fd2))];
                        else
                            comparison = [comparison, sprintf('  但其观察到的实际频移量(%.2f Hz)不一定大于目标2(%.2f Hz)。\n', abs(fd1), abs(fd2))];
                        end
                    else
                        comparison = [comparison, sprintf('目标1和目标2的速度大小相同(%.2f m/s)。\n', abs(v1))];
                         comparison = [comparison, sprintf('  目标1实际频移量(%.2f Hz)，目标2实际频移量(%.2f Hz)。\n', abs(fd1), abs(fd2))];
                    end
                    comparison = [comparison, sprintf('总体而言，速度越快的目标，其多普勒频移的绝对值通常越大。\n')];
                else
                    comparison = [comparison, sprintf('一个或两个目标的频移未能明确检测，无法进行详细的速度与频移对比。\n')];
                end
                 % 对时间延迟的对比可以类似添加，如果理论时延有意义
            else % 单目标
                if ~isnan(fd1)
                    comparison = [comparison, sprintf('目标1的速度为%.2f m/s，产生了%.2f Hz的实际频移。\n', v1, fd1)];
                else
                    comparison = [comparison, sprintf('目标1的速度为%.2f m/s，但未能明确检测到频移。\n', v1)];
                end
            end

            if isnan(delay1) && (strcmp(numTargetsStr, '2') && isnan(delay2))
                comparison = [comparison, sprintf('未能检测到任何目标的时间延迟。\n')];
            elseif isnan(fd1) && (strcmp(numTargetsStr, '2') && isnan(fd2))
                comparison = [comparison, sprintf('未能检测到任何目标的频移。\n')];
            end
            
            comparison = [comparison, sprintf('提示: 频谱和互相关峰值的检测依赖于信号强度、分离度和`findpeaks`参数设置。\n')];

            % 更新状态显示区
            app.StatusTextArea.Value = [freq_analysis, time_analysis, comparison];
        end


    end


    % Callbacks that handle component events
    methods (Access = private)

        % Button pushed function: GenerateConfigAndRunCButton
        function GenerateConfigAndRunCButtonPushed(app, event)

            % 生成配置文件
            app.StatusTextArea.Value = '正在生成配置文件...';

            % 获取参数
            signalType = app.SignalTypeDropDown.Value;
            frequency = app.FrequencyHzEditField.Value;
            pulseDuration = app.PulseDurationSEditField.Value;
            samplingRate = app.SamplingRateHzEditField.Value;
            lfmBandwidth = app.LFMBandwidthHzEditField.Value;
            soundSpeed = app.SoundSpeedMpsEditField.Value;
            numTargets = app.NumTargetsDropDown.Value;
            target1Velocity = app.Target1VelocityMpsEditField.Value;
            target1Amplitude = app.Target1InitialAmplitudeEditField.Value;
            target2Velocity = app.Target2VelocityMpsEditField.Value;
            target2Amplitude = app.Target2InitialAmplitudeEditField.Value;
            noiseEnabled = app.NoiseEnabledCheckBox.Value;
            snr = app.SNRdBEditField.Value;

            % 生成配置文件
            configFile = 'config.txt';
            fid = fopen(configFile, 'w');
            fprintf(fid, '# 仿真参数配置文件\n');
            fprintf(fid, 'SIGNAL_TYPE = %s\n', signalType);
            fprintf(fid, 'FREQUENCY_HZ = %f\n', frequency);
            fprintf(fid, 'PULSE_DURATION_S = %f\n', pulseDuration);
            fprintf(fid, 'SAMPLING_RATE_HZ = %f\n', samplingRate);
            fprintf(fid, 'LFM_BANDWIDTH_HZ = %f\n', lfmBandwidth);
            fprintf(fid, 'SOUND_SPEED_MPS = %f\n', soundSpeed);
            fprintf(fid, 'NUM_TARGETS = %d\n', numTargets);
            fprintf(fid, 'TARGET1_VELOCITY_MPS = %f\n', target1Velocity);
            fprintf(fid, 'TARGET1_INITIAL_AMPLITUDE = %f\n', target1Amplitude); % 这是修改后的行
            if strcmp(numTargets, '2')
                fprintf(fid, 'TARGET2_VELOCITY_MPS = %f\n', target2Velocity);
                % fprintf(fid, 'TARGET2_AMPLITUDE = %f\n', target2Amplitude); % 这是旧的行
                fprintf(fid, 'TARGET2_INITIAL_AMPLITUDE = %f\n', target2Amplitude); % 这是修改后的行
            end

            fprintf(fid, 'NOISE_ENABLED = %d\n', noiseEnabled);
            fprintf(fid, 'SNR_DB = %f\n', snr);
            fclose(fid);

            % 调用C程序
            app.StatusTextArea.Value = '配置文件已生成，正在调用C程序...';
            cProgramPath = '"E:\cwork\Project28_1\x64\Debug\Project28_1.exe'; % 更新路径
            [status, cmdout] = system(cProgramPath);

            if status == 0
                app.StatusTextArea.Value = 'C程序执行完毕，请点击“加载结果并绘图”按钮查看结果。';
            else
                app.StatusTextArea.Value = sprintf('C程序执行失败: %s', cmdout);
            end

        end

        % Button pushed function: LoadResultsAndPlotButton
        function LoadResultsAndPlotButtonPushed(app, event)
            % 加载数据并绘图
            app.StatusTextArea.Value = '正在加载数据...';

            try
                % 检查文件是否存在
                if ~isfile('original_signal.txt') || ~isfile('received_signal.txt')
                    error('数据文件不存在，请先运行C程序生成数据。');
                end

                % 加载数据文件
                originalSignal = readmatrix('original_signal.txt');
                receivedSignal = readmatrix('received_signal.txt');

                % 提取时间和信号数据
                timeOriginal = originalSignal(:, 1);
                signalOriginal = originalSignal(:, 2);
                timeReceived = receivedSignal(:, 1);
                signalReceived = receivedSignal(:, 2);

                % 检查数据是否为空
                if isempty(timeOriginal) || isempty(signalOriginal) || isempty(timeReceived) || isempty(signalReceived)
                    error('数据文件为空，请检查C程序的输出。');
                end

                % 计算FFT
                fs = app.SamplingRateHzEditField.Value;
                L = length(signalOriginal);
                f = fs*(0:(L/2))/L;
                Y_original = fft(signalOriginal);
                P2_original = abs(Y_original/L);
                P1_original = P2_original(1:L/2+1);
                P1_original(2:end-1) = 2*P1_original(2:end-1);

                Y_received = fft(signalReceived);
                P2_received = abs(Y_received/L);
                P1_received = P2_received(1:L/2+1);
                P1_received(2:end-1) = 2*P1_received(2:end-1);

                % 绘制时域信号
                plot(app.UIAxes_TimeDomain, timeOriginal, signalOriginal, 'b-', 'DisplayName', '原始信号');
                hold(app.UIAxes_TimeDomain, 'on');
                plot(app.UIAxes_TimeDomain, timeReceived, signalReceived, 'r-', 'DisplayName', '接收信号');
                hold(app.UIAxes_TimeDomain, 'off');
                legend(app.UIAxes_TimeDomain, 'show');
                title(app.UIAxes_TimeDomain, '时域信号');
                xlabel(app.UIAxes_TimeDomain, '时间 (s)');
                ylabel(app.UIAxes_TimeDomain, '幅度');

                % 绘制频域信号
                plot(app.UIAxes_FrequencyDomain, f, P1_original, 'b-', 'DisplayName', '原始信号频谱');
                hold(app.UIAxes_FrequencyDomain, 'on');
                plot(app.UIAxes_FrequencyDomain, f, P1_received, 'r-', 'DisplayName', '接收信号频谱');
                hold(app.UIAxes_FrequencyDomain, 'off');
                legend(app.UIAxes_FrequencyDomain, 'show');
                title(app.UIAxes_FrequencyDomain, '频域信号');
                xlabel(app.UIAxes_FrequencyDomain, '频率 (Hz)');
                ylabel(app.UIAxes_FrequencyDomain, '幅度');

                % 频域信号分析
                [fd1, fd2, theory_fd1, theory_fd2] = analyze_frequency_domain(app, f, P1_original, P1_received);

                % 时域信号分析
                [delay1, delay2, theory_delay1, theory_delay2] = analyze_time_domain(app, timeOriginal, signalOriginal, timeReceived, signalReceived);

                % 生成分析结果
                generate_analysis_result(app, fd1, fd2, theory_fd1, theory_fd2, delay1, delay2, theory_delay1, theory_delay2);
            catch ME
                app.StatusTextArea.Value = sprintf('加载数据失败: %s', ME.message);
            end
        end

        % Button pushed function: ClearParametersButton
        function ClearParametersButtonPushed(app, event)
            % 清空参数输入控件
            app.SignalTypeDropDown.Value = 'CW';
            app.FrequencyHzEditField.Value = 1000;
            app.PulseDurationSEditField.Value = 1;
            app.SamplingRateHzEditField.Value = 10000;
            app.LFMBandwidthHzEditField.Value = 500;
            app.SoundSpeedMpsEditField.Value = 1500;
            app.NumTargetsDropDown.Value = '1';
            app.Target1VelocityMpsEditField.Value = 10;
            app.Target1InitialAmplitudeEditField.Value = 1;
            app.Target2VelocityMpsEditField.Value = -15;
            app.Target2InitialAmplitudeEditField.Value = 0.8;
            app.NoiseEnabledCheckBox.Value = true;
            app.SNRdBEditField.Value = 10;

            % 清空绘图显示区的图像
            cla(app.UIAxes_TimeDomain);
            cla(app.UIAxes_FrequencyDomain);

            % 更新状态显示区
            app.StatusTextArea.Value = '参数已清空，图像已清除。';

        end

        % Value changed function: SignalTypeDropDown
        function SignalTypeDropDownValueChanged(app, event)
            newSignalType = app.SignalTypeDropDown.Value;
            if strcmp(newSignalType, 'LFM')
                app.LFMBandwidthHzEditField.Enable = 'on';
            else
                app.LFMBandwidthHzEditField.Enable = 'off';
            end
        end

        % Value changed function: NumTargetsDropDown
        function NumTargetsDropDownValueChanged(app, event)
            % 获取目标数量的值
            numTargets = app.NumTargetsDropDown.Value;

            % 判断目标数量是否为2
            if strcmp(numTargets, '2')
                % 启用目标2相关字段
                app.Target2VelocityMpsEditField.Enable = 'on';
                app.Target2InitialAmplitudeEditField.Enable = 'on';
            else
                % 禁用目标2相关字段
                app.Target2VelocityMpsEditField.Enable = 'off';
                app.Target2InitialAmplitudeEditField.Enable = 'off';
            end
        end

        % Value changed function: NoiseEnabledCheckBox
        function NoiseEnabledCheckBoxValueChanged(app, event)
            if app.NoiseEnabledCheckBox.Value
                app.SNRdBEditField.Enable = 'on';
            else
                app.SNRdBEditField.Enable = 'off';
            end
        end
    end

    % Component initialization
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Create UIFigure and hide until all components are created
            app.UIFigure = uifigure('Visible', 'off');
            app.UIFigure.Position = [100 100 999 927];
            app.UIFigure.Name = 'MATLAB App';

            % Create ParametersPanel
            app.ParametersPanel = uipanel(app.UIFigure);
            app.ParametersPanel.Title = '参数输入';
            app.ParametersPanel.Position = [85 391 444 290];

            % Create Label_2
            app.Label_2 = uilabel(app.ParametersPanel);
            app.Label_2.HorizontalAlignment = 'right';
            app.Label_2.Position = [32 236 56 22];
            app.Label_2.Text = '信号类型:';

            % Create SignalTypeDropDown
            app.SignalTypeDropDown = uidropdown(app.ParametersPanel);
            app.SignalTypeDropDown.Items = {'CW', 'LFM'};
            app.SignalTypeDropDown.ValueChangedFcn = createCallbackFcn(app, @SignalTypeDropDownValueChanged, true);
            app.SignalTypeDropDown.Position = [102 236 100 22];
            app.SignalTypeDropDown.Value = 'CW';

            % Create HzLabel_2
            app.HzLabel_2 = uilabel(app.ParametersPanel);
            app.HzLabel_2.HorizontalAlignment = 'right';
            app.HzLabel_2.Position = [230 236 58 22];
            app.HzLabel_2.Text = '频率 (Hz):';

            % Create FrequencyHzEditField
            app.FrequencyHzEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.FrequencyHzEditField.Limits = [1 10000000];
            app.FrequencyHzEditField.Position = [303 236 100 22];
            app.FrequencyHzEditField.Value = 1000;

            % Create sLabel_2
            app.sLabel_2 = uilabel(app.ParametersPanel);
            app.sLabel_2.HorizontalAlignment = 'right';
            app.sLabel_2.Position = [32 188 74 22];
            app.sLabel_2.Text = '脉冲时长 (s):';

            % Create PulseDurationSEditField
            app.PulseDurationSEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.PulseDurationSEditField.Limits = [1e-06 10];
            app.PulseDurationSEditField.Position = [121 188 100 22];
            app.PulseDurationSEditField.Value = 1;

            % Create HzEditFieldLabel
            app.HzEditFieldLabel = uilabel(app.ParametersPanel);
            app.HzEditFieldLabel.HorizontalAlignment = 'right';
            app.HzEditFieldLabel.Position = [230 188 70 22];
            app.HzEditFieldLabel.Text = '采样率 (Hz):';

            % Create SamplingRateHzEditField
            app.SamplingRateHzEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.SamplingRateHzEditField.Limits = [1 100000000];
            app.SamplingRateHzEditField.Position = [315 188 100 22];
            app.SamplingRateHzEditField.Value = 10000;

            % Create LFMHzEditFieldLabel
            app.LFMHzEditFieldLabel = uilabel(app.ParametersPanel);
            app.LFMHzEditFieldLabel.HorizontalAlignment = 'right';
            app.LFMHzEditFieldLabel.Position = [32 151 82 22];
            app.LFMHzEditFieldLabel.Text = 'LFM带宽 (Hz):';

            % Create LFMBandwidthHzEditField
            app.LFMBandwidthHzEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.LFMBandwidthHzEditField.Limits = [0 10000000];
            app.LFMBandwidthHzEditField.Position = [129 151 100 22];
            app.LFMBandwidthHzEditField.Value = 500;

            % Create msEditFieldLabel
            app.msEditFieldLabel = uilabel(app.ParametersPanel);
            app.msEditFieldLabel.HorizontalAlignment = 'right';
            app.msEditFieldLabel.Position = [237 151 63 22];
            app.msEditFieldLabel.Text = '声速 (m/s):';

            % Create SoundSpeedMpsEditField
            app.SoundSpeedMpsEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.SoundSpeedMpsEditField.Limits = [1 5000];
            app.SoundSpeedMpsEditField.Position = [315 151 100 22];
            app.SoundSpeedMpsEditField.Value = 1500;

            % Create Label_3
            app.Label_3 = uilabel(app.ParametersPanel);
            app.Label_3.HorizontalAlignment = 'right';
            app.Label_3.Position = [31 111 56 22];
            app.Label_3.Text = '目标数量:';

            % Create NumTargetsDropDown
            app.NumTargetsDropDown = uidropdown(app.ParametersPanel);
            app.NumTargetsDropDown.Items = {'1', '2'};
            app.NumTargetsDropDown.ValueChangedFcn = createCallbackFcn(app, @NumTargetsDropDownValueChanged, true);
            app.NumTargetsDropDown.Position = [102 111 100 22];
            app.NumTargetsDropDown.Value = '1';

            % Create msEditFieldLabel_2
            app.msEditFieldLabel_2 = uilabel(app.ParametersPanel);
            app.msEditFieldLabel_2.HorizontalAlignment = 'right';
            app.msEditFieldLabel_2.Position = [228 111 94 22];
            app.msEditFieldLabel_2.Text = '目标1速度 (m/s):';

            % Create Target1VelocityMpsEditField
            app.Target1VelocityMpsEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.Target1VelocityMpsEditField.Position = [337 111 100 22];
            app.Target1VelocityMpsEditField.Value = 10;

            % Create msEditFieldLabel_3
            app.msEditFieldLabel_3 = uilabel(app.ParametersPanel);
            app.msEditFieldLabel_3.HorizontalAlignment = 'right';
            app.msEditFieldLabel_3.Enable = 'off';
            app.msEditFieldLabel_3.Position = [228 68 94 22];
            app.msEditFieldLabel_3.Text = '目标2速度 (m/s):';

            % Create Target2VelocityMpsEditField
            app.Target2VelocityMpsEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.Target2VelocityMpsEditField.Enable = 'off';
            app.Target2VelocityMpsEditField.Position = [337 68 100 22];
            app.Target2VelocityMpsEditField.Value = -15;

            % Create Label_4
            app.Label_4 = uilabel(app.ParametersPanel);
            app.Label_4.HorizontalAlignment = 'right';
            app.Label_4.Enable = 'off';
            app.Label_4.Position = [19 24 87 22];
            app.Label_4.Text = '目标2初始幅度:';

            % Create Target2InitialAmplitudeEditField
            app.Target2InitialAmplitudeEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.Target2InitialAmplitudeEditField.Enable = 'off';
            app.Target2InitialAmplitudeEditField.Position = [121 24 100 22];
            app.Target2InitialAmplitudeEditField.Value = 0.8;

            % Create NoiseEnabledCheckBox
            app.NoiseEnabledCheckBox = uicheckbox(app.ParametersPanel);
            app.NoiseEnabledCheckBox.ValueChangedFcn = createCallbackFcn(app, @NoiseEnabledCheckBoxValueChanged, true);
            app.NoiseEnabledCheckBox.Text = '启用噪声';
            app.NoiseEnabledCheckBox.Position = [337 3 70 22];

            % Create Label_5
            app.Label_5 = uilabel(app.ParametersPanel);
            app.Label_5.HorizontalAlignment = 'right';
            app.Label_5.Position = [19 68 87 22];
            app.Label_5.Text = '目标1初始幅度:';

            % Create Target1InitialAmplitudeEditField
            app.Target1InitialAmplitudeEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.Target1InitialAmplitudeEditField.Position = [121 68 100 22];
            app.Target1InitialAmplitudeEditField.Value = 1;

            % Create dBEditFieldLabel
            app.dBEditFieldLabel = uilabel(app.ParametersPanel);
            app.dBEditFieldLabel.HorizontalAlignment = 'right';
            app.dBEditFieldLabel.Position = [237 38 70 22];
            app.dBEditFieldLabel.Text = '信噪比 (dB):';

            % Create SNRdBEditField
            app.SNRdBEditField = uieditfield(app.ParametersPanel, 'numeric');
            app.SNRdBEditField.Enable = 'off';
            app.SNRdBEditField.Position = [322 38 100 22];
            app.SNRdBEditField.Value = 10;

            % Create ControlsPanel
            app.ControlsPanel = uipanel(app.UIFigure);
            app.ControlsPanel.Title = '控制操作';
            app.ControlsPanel.Position = [530 391 344 290];

            % Create GenerateConfigAndRunCButton
            app.GenerateConfigAndRunCButton = uibutton(app.ControlsPanel, 'push');
            app.GenerateConfigAndRunCButton.ButtonPushedFcn = createCallbackFcn(app, @GenerateConfigAndRunCButtonPushed, true);
            app.GenerateConfigAndRunCButton.Position = [65 216 131 22];
            app.GenerateConfigAndRunCButton.Text = '生成配置并运行C程序';

            % Create LoadResultsAndPlotButton
            app.LoadResultsAndPlotButton = uibutton(app.ControlsPanel, 'push');
            app.LoadResultsAndPlotButton.ButtonPushedFcn = createCallbackFcn(app, @LoadResultsAndPlotButtonPushed, true);
            app.LoadResultsAndPlotButton.Position = [81 167 100 22];
            app.LoadResultsAndPlotButton.Text = '加载结果并绘图';

            % Create ClearParametersButton
            app.ClearParametersButton = uibutton(app.ControlsPanel, 'push');
            app.ClearParametersButton.ButtonPushedFcn = createCallbackFcn(app, @ClearParametersButtonPushed, true);
            app.ClearParametersButton.Position = [81 105 100 22];
            app.ClearParametersButton.Text = '清空参数';

            % Create StatusPanel
            app.StatusPanel = uipanel(app.UIFigure);
            app.StatusPanel.Title = '状态显示';
            app.StatusPanel.Position = [86 680 789 169];

            % Create StatusTextArea
            app.StatusTextArea = uitextarea(app.StatusPanel);
            app.StatusTextArea.Editable = 'off';
            app.StatusTextArea.HorizontalAlignment = 'center';
            app.StatusTextArea.FontWeight = 'bold';
            app.StatusTextArea.FontAngle = 'italic';
            app.StatusTextArea.Position = [90 0 612 148];
            app.StatusTextArea.Value = {'欢迎使用！请设置参数并运行。'};

            % Create PlotsPanel
            app.PlotsPanel = uipanel(app.UIFigure);
            app.PlotsPanel.Title = '绘图显示';
            app.PlotsPanel.Position = [85 53 789 339];

            % Create UIAxes_FrequencyDomain
            app.UIAxes_FrequencyDomain = uiaxes(app.PlotsPanel);
            title(app.UIAxes_FrequencyDomain, '频域谱线')
            xlabel(app.UIAxes_FrequencyDomain, {'频率 (Hz)'; ''})
            ylabel(app.UIAxes_FrequencyDomain, '幅度')
            zlabel(app.UIAxes_FrequencyDomain, 'Z')
            app.UIAxes_FrequencyDomain.TitleFontWeight = 'bold';
            app.UIAxes_FrequencyDomain.Position = [414 38 365 247];

            % Create UIAxes_TimeDomain
            app.UIAxes_TimeDomain = uiaxes(app.PlotsPanel);
            title(app.UIAxes_TimeDomain, '时域波形')
            xlabel(app.UIAxes_TimeDomain, '时间 (s)')
            ylabel(app.UIAxes_TimeDomain, '幅度')
            zlabel(app.UIAxes_TimeDomain, 'Z')
            app.UIAxes_TimeDomain.TitleFontWeight = 'bold';
            app.UIAxes_TimeDomain.Position = [16 53 386 232];

            % Create TextArea2
            app.TextArea2 = uitextarea(app.UIFigure);
            app.TextArea2.HorizontalAlignment = 'center';
            app.TextArea2.FontSize = 24;
            app.TextArea2.FontWeight = 'bold';
            app.TextArea2.Position = [175 861 612 47];
            app.TextArea2.Value = {'声学仿真控制面板'};

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = DopplerSimulationApp_exported

            % Create UIFigure and components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            if nargout == 0
                clear app
            end
        end

        % Code that executes before app deletion
        function delete(app)

            % Delete UIFigure when app is deleted
            delete(app.UIFigure)
        end
    end
end