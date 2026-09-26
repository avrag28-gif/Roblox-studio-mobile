package com.rsm.mobile;
import android.security.keystore.KeyGenParameterSpec;
import android.security.keystore.KeyProperties;
import android.util.Base64;
import java.nio.charset.StandardCharsets;
import java.security.KeyStore;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
public final class CredentialStore {
  private static final String KS="AndroidKeyStore",ALIAS="RSM_CREDENTIAL_KEY";
  private static SecretKey key() throws Exception {
    KeyStore ks=KeyStore.getInstance(KS);ks.load(null);
    if(ks.containsAlias(ALIAS))return ((KeyStore.SecretKeyEntry)ks.getEntry(ALIAS,null)).getSecretKey();
    KeyGenerator g=KeyGenerator.getInstance(KeyProperties.KEY_ALGORITHM_AES,KS);
    g.init(new KeyGenParameterSpec.Builder(ALIAS,KeyProperties.PURPOSE_ENCRYPT|KeyProperties.PURPOSE_DECRYPT).setBlockModes(KeyProperties.BLOCK_MODE_GCM).setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE).build());
    return g.generateKey();
  }
  public static String encrypt(String value) {
    try{Cipher c=Cipher.getInstance("AES/GCM/NoPadding");c.init(Cipher.ENCRYPT_MODE,key());byte[] iv=c.getIV(),ct=c.doFinal(value.getBytes(StandardCharsets.UTF_8));byte[] all=new byte[iv.length+ct.length];System.arraycopy(iv,0,all,0,iv.length);System.arraycopy(ct,0,all,iv.length,ct.length);return Base64.encodeToString(all,Base64.NO_WRAP);}catch(Exception e){return "";}
  }
  public static String decrypt(String encoded) {
    try{byte[] all=Base64.decode(encoded,Base64.NO_WRAP);byte[] iv=new byte[12],ct=new byte[all.length-12];System.arraycopy(all,0,iv,0,12);System.arraycopy(all,12,ct,0,ct.length);Cipher c=Cipher.getInstance("AES/GCM/NoPadding");c.init(Cipher.DECRYPT_MODE,key(),new javax.crypto.spec.GCMParameterSpec(128,iv));return new String(c.doFinal(ct),StandardCharsets.UTF_8);}catch(Exception e){return "";}
  }
}