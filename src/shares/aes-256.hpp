#pragma once
#include <array>
#include <functional>
#include <iostream>
#include <botan/gcm.h>
#include <botan/ocb.h>
#include <botan/sha3.h>

#ifndef __AES_256_HPP__
#define __AES_256_HPP__

class encryption_base
{
protected:
	const std::string associated_data = "UDP PortHopping";
	const std::string empty_error_message = "Empty Input Data";
	const size_t CACHE_SIZE = 4096;

	std::array<uint8_t, 32> key;
	Botan::secure_vector<uint8_t> iv;

	std::unique_ptr<Botan::AEAD_Mode> encoder;
	std::unique_ptr<Botan::AEAD_Mode> decoder;

	static Botan::secure_vector<uint8_t> head_tail_xor(const Botan::secure_vector<uint8_t> &input_vector)
	{
		size_t half_size = input_vector.size() / 2;
		Botan::secure_vector<uint8_t> output(half_size);
		std::vector<uint8_t> first_half(half_size);
		std::vector<uint8_t> second_half(half_size);
		std::copy_n(input_vector.begin(), half_size, first_half.begin());
		std::copy_n(input_vector.rbegin(), half_size, second_half.begin());
		std::transform(first_half.begin(), first_half.end(), second_half.begin(), output.begin(), std::bit_xor<uint8_t>());
		return output;
	}

	static void head_tail_xor(const Botan::secure_vector<uint8_t> &input_vector, Botan::secure_vector<uint8_t> &output)
	{
		size_t half_size = input_vector.size() / 2;
		std::vector<uint8_t> first_half(half_size);
		std::vector<uint8_t> second_half(half_size);
		std::copy_n(input_vector.begin(), half_size, first_half.begin());
		std::copy_n(input_vector.rbegin(), half_size, second_half.begin());
		std::transform(first_half.begin(), first_half.end(), second_half.begin(), output.begin(), std::bit_xor<uint8_t>());
	}

public:
	template<typename T>
	T encrypt(const T &input_plain_data, std::string &error_message)
	{
		if (input_plain_data.empty())
		{
			error_message = empty_error_message;
			return T();
		}

		try
		{
			thread_local Botan::secure_vector<uint8_t> secure_data(CACHE_SIZE);
			secure_data.resize(input_plain_data.size());
			std::copy_n((const uint8_t *)input_plain_data.data(), input_plain_data.size(), secure_data.data());

			encoder->start(iv);
			encoder->finish(secure_data);

			T output_cipher((typename T::pointer)secure_data.data(), (typename T::pointer)secure_data.data() + secure_data.size());
			return output_cipher;
		}
		catch (std::exception &e)
		{
			error_message = e.what();
		}

		return T();
	}

	template<typename T>
	T decrypt(const T &input_plain_data, std::string &error_message)
	{
		if (input_plain_data.empty())
		{
			error_message = empty_error_message;
			return T();
		}

		try
		{
			thread_local Botan::secure_vector<uint8_t> secure_data(CACHE_SIZE);
			secure_data.resize(input_plain_data.size());
			std::copy_n((const uint8_t *)input_plain_data.data(), input_plain_data.size(), secure_data.data());

			decoder->start(iv);
			decoder->finish(secure_data);

			T output_plain((typename T::pointer)secure_data.data(), (typename T::pointer)secure_data.data() + secure_data.size());
			return output_plain;
		}
		catch (std::exception &e)
		{
			error_message = e.what();
		}

		return T();
	}

	template<typename T>
	T encrypt(T &&input_plain_data, std::string &error_message)
	{
		if (input_plain_data.empty())
		{
			error_message = empty_error_message;
			return T();
		}

		try
		{
			thread_local Botan::secure_vector<uint8_t> secure_data(CACHE_SIZE);
			secure_data.resize(input_plain_data.size());
			std::copy_n((const uint8_t *)input_plain_data.data(), input_plain_data.size(), secure_data.data());

			encoder->start(iv);
			encoder->finish(secure_data);

			input_plain_data.assign((typename T::pointer)secure_data.data(), (typename T::pointer)secure_data.data() + secure_data.size());
			return input_plain_data;
		}
		catch (std::exception &e)
		{
			error_message = e.what();
		}

		return T();
	}

	template<typename T>
	T decrypt(T &&input_plain_data, std::string &error_message)
	{
		if (input_plain_data.empty())
		{
			error_message = empty_error_message;
			return T();
		}

		try
		{
			thread_local Botan::secure_vector<uint8_t> secure_data(CACHE_SIZE);
			secure_data.resize(input_plain_data.size());
			std::copy_n((const uint8_t *)input_plain_data.data(), input_plain_data.size(), secure_data.data());

			decoder->start(iv);
			decoder->finish(secure_data);

			input_plain_data.assign((typename T::pointer)secure_data.data(), (typename T::pointer)secure_data.data() + secure_data.size());
			return input_plain_data;
		}
		catch (std::exception &e)
		{
			error_message = e.what();
		}

		return T();
	}

	std::string encrypt(const uint8_t *input_plain_data, size_t length, uint8_t *output_cipher, size_t &output_legnth)
	{
		if (length == 0)
		{
			output_legnth = 0;
			return empty_error_message;
		}
		
		std::string error_message;
		try
		{
			thread_local Botan::secure_vector<uint8_t> secure_data(CACHE_SIZE);
			secure_data.resize(length);
			std::copy_n(input_plain_data, length, secure_data.data());

			encoder->start(iv);
			encoder->finish(secure_data);

			std::copy(secure_data.begin(), secure_data.end(), output_cipher);
			output_legnth = secure_data.size();
		}
		catch (std::exception &e)
		{
			output_legnth = 0;
			error_message = e.what();
		}

		return error_message;
	}

	std::string decrypt(const uint8_t *input_cipher_data, size_t length, uint8_t *output_plain_data, size_t &output_legnth)
	{
		if (length == 0)
		{
			return empty_error_message;
		}
		
		std::string error_message;
		try
		{
			thread_local Botan::secure_vector<uint8_t> secure_data(CACHE_SIZE);
			secure_data.resize(length);
			std::copy_n(input_cipher_data, length, secure_data.data());

			decoder->start(iv);
			decoder->finish(secure_data);

			std::copy(secure_data.begin(), secure_data.end(), output_plain_data);
			output_legnth = secure_data.size();
		}
		catch (std::exception &e)
		{
			output_legnth = 0;
			error_message = e.what();
		}

		return error_message;
	}
};

class aes_256_gcm : public encryption_base
{
private:
	void set_key(const std::string &input_key)
	{
		iv.resize(16);
		if (input_key.size() == 0)
		{
			key.fill(0);
			std::fill(iv.begin(), iv.end(), 0);
			return;
		}

		Botan::SHA_3_256 sha3;
		Botan::secure_vector<uint8_t> output_key = sha3.process((const uint8_t *)input_key.c_str(), input_key.size());
		std::copy(output_key.begin(), output_key.end(), key.begin());

		head_tail_xor(output_key, iv);

		encoder = Botan::GCM_Mode::create("AES-256/GCM", Botan::ENCRYPTION);
		decoder = Botan::GCM_Mode::create("AES-256/GCM", Botan::DECRYPTION);

		encoder->set_key(key.data(), key.size());
		encoder->set_associated_data((const uint8_t *)associated_data.c_str(), associated_data.size());

		decoder->set_key(key.data(), key.size());
		decoder->set_associated_data((const uint8_t *)associated_data.c_str(), associated_data.size());
	}

public:
	aes_256_gcm() = delete;

	aes_256_gcm(aes_256_gcm &&other) noexcept
	{
		key = std::move(other.key);
		iv = std::move(other.iv);
		encoder = std::move(other.encoder);
		decoder = std::move(other.decoder);
	}

	aes_256_gcm(const std::string &input_key)
	{
		set_key(input_key);
	}

	aes_256_gcm& operator=(aes_256_gcm &&other) noexcept
	{
		key = std::move(other.key);
		iv = std::move(other.iv);
		encoder = std::move(other.encoder);
		decoder = std::move(other.decoder);
		return *this;
	}
};

class aes_256_ocb : public encryption_base
{
private:
	void set_key(const std::string &input_key)
	{
		iv.resize(12);
		if (input_key.size() == 0)
		{
			key.fill(0);
			std::fill(iv.begin(), iv.end(), 0);
			return;
		}

		Botan::SHA_3_256 sha3_256;
		Botan::secure_vector<uint8_t> output_key = sha3_256.process((const uint8_t *)input_key.c_str(), input_key.size());
		std::copy(output_key.begin(), output_key.end(), key.begin());

		Botan::SHA_3_384 sha3_384;
		Botan::secure_vector<uint8_t> output_key_384 = sha3_384.process((const uint8_t *)input_key.c_str(), input_key.size());

		Botan::secure_vector<uint8_t> output = head_tail_xor(output_key_384);
		while (output.size() > iv.size())
		{
			auto new_output = head_tail_xor(output);
			output.resize(new_output.size());
			std::copy(new_output.begin(), new_output.end(), output.begin());
		}

		std::copy(output.begin(), output.end(), iv.begin());

		encoder = Botan::GCM_Mode::create("AES-256/OCB", Botan::ENCRYPTION);
		decoder = Botan::GCM_Mode::create("AES-256/OCB", Botan::DECRYPTION);

		encoder->set_key(key.data(), key.size());
		encoder->set_associated_data((const uint8_t *)associated_data.c_str(), associated_data.size());

		decoder->set_key(key.data(), key.size());
		decoder->set_associated_data((const uint8_t *)associated_data.c_str(), associated_data.size());
	}

public:
	aes_256_ocb() = delete;

	aes_256_ocb(aes_256_ocb &&other) noexcept
	{
		key = std::move(other.key);
		iv = std::move(other.iv);
		encoder = std::move(other.encoder);
		decoder = std::move(other.decoder);
	}

	aes_256_ocb(const std::string &input_key)
	{
		set_key(input_key);
	}

	aes_256_ocb& operator=(aes_256_ocb &&other) noexcept
	{
		key = std::move(other.key);
		iv = std::move(other.iv);
		encoder = std::move(other.encoder);
		decoder = std::move(other.decoder);
		return *this;
	}
};


#endif